#include "openmc/transient_source.h"
#include "openmc/hdf5_interface.h"
#include "openmc/source.h"

#include "hdf5.h"

#include <cstring>
#include <iostream>
namespace openmc {
namespace simulation {
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {0.0};
bool time_slice_bank_written = false;
} // namespace simulation

// function to check for both tallies existence once the file is opened, and to
// return fatal errors if they do not
void check_tally_exists(hid_t obj_id)
{
  // first check to ensure the tally data exists
  const std::string path_to_tally1 = "/tallies/tally 1";
  const std::string path_to_tally2 = "/tallies/tally 2";

  if (!object_exists(obj_id, path_to_tally1.c_str()))
    fatal_error("Delayed Neutron Precursor tally was not set, the transient "
                "source cannot be constructed!");

  if (!object_exists(obj_id, path_to_tally2.c_str()))
    fatal_error("The neutron concentration tally was not set, the transient "
                "source cannot be constructed!");
}

// function to check if the tallies are of the correct type, if not, return a
// fatal error
void check_correct_tallies(hid_t obj_id_dnp, hid_t obj_id_neutron)
{
  // first we need grab the appropriate datasets
  hid_t data_dnp = H5Dopen2(obj_id_dnp, "score_bins", H5P_DEFAULT);
  hid_t data_neutron = H5Dopen2(obj_id_neutron, "score_bins", H5P_DEFAULT);

  // grab the datatypes of our datasets
  hid_t datatype_dnp = H5Dget_type(data_dnp);
  hid_t datatype_neutron = H5Dget_type(data_neutron);

  // get the datasize of our data
  hsize_t datasize_dnp = H5Tget_size(datatype_dnp);
  hsize_t datasize_neutron = H5Tget_size(datatype_neutron);

  // allocate string buffers
  char* dnp_val = (char*)malloc(datasize_dnp + 1);
  char* neutron_val = (char*)malloc(datasize_neutron + 1);

  // read in the value
  H5Dread(data_dnp, datatype_dnp, H5S_ALL, H5S_ALL, H5P_DEFAULT, dnp_val);
  H5Dread(
    data_neutron, datatype_neutron, H5S_ALL, H5S_ALL, H5P_DEFAULT, neutron_val);

  // add null terminator
  dnp_val[datasize_dnp] = '\0';
  neutron_val[datasize_neutron] = '\0';

  // perform checks and return with fatal errors if needed
  if (strcmp(dnp_val, "precursors") != 0)
    fatal_error("The first tally must be set to precursor scoring in order to "
                "generate the transient source!");

  if (strcmp(neutron_val, "neutron-density") != 0)
    fatal_error("The second tally must be set to neutron density scoring in "
                "order to generate the transient source!");

  // free memory and close HDF5 components
  free(dnp_val);
  free(neutron_val);
  H5Dclose(data_dnp);
  H5Dclose(data_neutron);
  H5Tclose(datatype_dnp);
  H5Tclose(datatype_neutron);
}

// returns the size of the 1D array tallies
void get_tally_shape(hid_t obj_id, hsize_t* dims)
{
  hid_t d_space;
  d_space = H5Dget_space(obj_id);
  H5Sget_simple_extent_dims(d_space, dims, nullptr);
  H5Sclose(d_space);
}

// reads tally results to a buffer of data
void read_tally_data(hid_t obj_id, hsize_t* dims, const int ndims, double* data)
{
  // set the dataspace and hyperslab parameters
  hid_t d_space = H5Dget_space(obj_id);
  hsize_t start[3], count[3], stride[3], block[3];

  // allocate hyperslab components
  for (int i = 0; i < ndims; i++) {
    start[i] = 0;
    count[i] = 1;
    block[i] = 1;
    stride[i] = 1;
  }
  count[0] = dims[0];

  // select the hyperslab to read
  H5Sselect_hyperslab(d_space, H5S_SELECT_SET, start, stride, count, block);

  // create the dataspace for the selected memory
  hid_t memory_space = H5Screate_simple(1, count, NULL);

  // read in the tally information to the buffer
  H5Dread(obj_id, H5T_IEEE_F64LE, memory_space, d_space, H5P_DEFAULT, data);

  // close dataspaces
  H5Sclose(memory_space);
  H5Sclose(d_space);
}

// function to create the dnp/neutron ratios needed for source normalization
vector<double> create_dnp_neutron_ratios(
  const vector<double>& dnps, const vector<double>& neutrons)
{
  vector<double> ratios(dnps.size());
  int neutron_counter = 0;
  constexpr int num_delayed_groups = 6;
  for (int i = 0; i < dnps.size(); i++) {
    if (i % num_delayed_groups == 0 and i != 0) {
      neutron_counter++;
    }
    ratios[i] = dnps[i] / neutrons[neutron_counter];
  }
  return ratios;
}

// create a normalized discrete pdf of neutron concentrations, normalized to 1.
vector<double> create_neutron_pdf(const vector<double>& neutrons)
{
  vector<double> pdf;
  double sum = 0.0;
  for (double value : neutrons) {
    sum += value;
  }
  for (int i = 0; i < neutrons.size(); i++) {
    double val = (neutrons[i] / sum);
    pdf.emplace_back(val);
  }
  return pdf;
}

// function to use the dnp/n ratios and pdf to create the problem normalized
// precursor concentrations
vector<double> calculate_normalized_precursors(
  const vector<double>& ratios, const vector<double>& pdf)
{
  vector<double> normalized_precursors;
  constexpr int num_delayed_groups = 6;
  int neutron_counter = 0;
  for (int i = 0; i < ratios.size(); i++) {
    if (i % num_delayed_groups == 0 and i != 0) {
      neutron_counter++;
    }
    normalized_precursors.emplace_back(ratios[i] * pdf[neutron_counter]);
  }
  return normalized_precursors;
}

// function to write normalized precursors to the transient source file.
void write_out_precursors(const vector<double>& precursors)
{
  // load the source file
  const std::string sourcefile = "transient_source.h5";
  hid_t source_file = file_open(sourcefile, 'a', false);

  // Create the dataspace for our 1D vector of precursor concentrations
  constexpr int ndims = 1;
  const hsize_t dims = {precursors.size()};

  hid_t prec_dataspace = H5Screate_simple(ndims, &dims, NULL);

  // Create the dataset
  hid_t prec_dataset = H5Dcreate(source_file, "/precursor_concentrations",
    H5T_NATIVE_DOUBLE, prec_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // Write the dataset
  H5Dwrite(prec_dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
    precursors.data());

  // Close managed resources
  H5Sclose(prec_dataspace);
  H5Dclose(prec_dataset);
  file_close(source_file);
}

// function to write out the mesh to the transient source file.
void write_out_mesh(hid_t statepoint_file)
{
  // load the source file
  const std::string filename = "transient_source.h5";
  hid_t source_file = file_open(filename, 'a', false);

  // get the mesh from the statepoint and copy it to the transient_source file
  H5Ocopy(statepoint_file, "/tallies/meshes/mesh 1", source_file, "/prec_mesh",
    H5P_DEFAULT, H5P_DEFAULT);

  // close the opened source file
  file_close(source_file);
}

// utility function to read in vector double values from the source file 
vector<double> read_data(const std::string& sourcefile, const std::string& attr, bool within_mesh) {

  // read the source file and get the dataset 
  hid_t file_obj = file_open(sourcefile, 'r', false);
  hid_t dataset = 0;
  hid_t prec_group = 0;
  if (within_mesh){
    prec_group = H5Gopen2(file_obj, "prec_mesh", H5P_DEFAULT);
    dataset = H5Dopen2(prec_group, attr.c_str(), H5P_DEFAULT);
  } else {
    dataset = H5Dopen2(file_obj, attr.c_str(), H5P_DEFAULT);
  }
  // get the dimensions
  hid_t dspace = H5Dget_space(dataset); 
  constexpr int ndims = 1; // assume we have a 1D vector of data to read
  hsize_t dims[ndims];
  H5Sget_simple_extent_dims(dspace, dims, NULL);
  
  // read data to vector
  vector<double> data(dims[0]);
  H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
  
  // free memory
  H5Sclose(dspace);
  if (within_mesh){
    H5Gclose(prec_group); 
  }
  H5Dclose(dataset);
  file_close(file_obj);

  // return data
  return data;
}

// function to read in the precursor vector from the transient_source.h5 file
vector<double> read_precursor_concentrations(const std::string& sourcefile) {
  const std::string group_name = "precursor_concentrations";
  vector<double> prec_concentrations = read_data(sourcefile, group_name, false);
  return prec_concentrations; 
}

// function to get time-sliced particle sites from transient source
vector<SourceSite> read_timeslice_source(const std::string& sourcefile){
  FileSource source = FileSource(sourcefile);
  vector<SourceSite> sites = source.get_sites_from_file();
  return sites; 
}

// get the type of mesh from the prec_mesh group in the transient_source.h5 file
const std::string get_mesh_type(const std::string& sourcefile){
  // read in the dataset to determine the type
  hid_t file_id = file_open(sourcefile, 'r', false);
  hid_t mesh_group = H5Gopen2(file_id, "prec_mesh", H5P_DEFAULT);
  hid_t mesh_type = H5Dopen2(mesh_group, "type", H5P_DEFAULT);

  // get dimensions and allocate buffers
  hid_t mesh_datatype = H5Dget_type(mesh_type);
  hsize_t mesh_datasize = H5Tget_size(mesh_datatype);
  char* type_of_mesh = (char*)malloc(mesh_datasize + 1);

  // read in the value
  H5Dread(mesh_type, mesh_datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, type_of_mesh);
  type_of_mesh[mesh_datasize] = '\0';

  const std::string mesh_type_string = std::string(type_of_mesh);

  // free memory
  free(type_of_mesh);
  H5Dclose(mesh_type);
  H5Tclose(mesh_datatype);
  H5Gclose(mesh_group);
  file_close(file_id); 

  // return the value
  return mesh_type_string; 
}

// function to get a spherical precursor mesh from file
SphericalMesh get_spherical_precmesh(const std::string& sourcefile) {
  vector<double> r = read_data(sourcefile, "r_grid", true);
  vector<double> theta = read_data(sourcefile, "theta_grid", true);
  vector<double> phi = read_data(sourcefile, "phi_grid", true);
  array<double, 3> arr_origin = {0.0, 0.0, 0.0};
  Position origin = arr_origin; 
  SphericalMesh precursor_mesh = SphericalMesh(r, theta, phi, origin);
  return precursor_mesh; 
}


// routine to finalize the transient_source.h5 file needed for dynamic simulation
void finalize_transient_source()
{
  // dataspace dimensions
  constexpr int ndims = 3;

  // error out if no time_slice source
  if (!simulation::time_slice_bank_written) {
    fatal_error("A time-slice neutron source must be used to normalize the "
                "transient source!");
  }

  // open transient statepoint file
  const std::string filename = "transient_statepoint.h5";
  hid_t file_id = file_open(filename, 'r', false);

  // check for the existence of the necessary tallies
  check_tally_exists(file_id);

  // get precursor and neutron group handles
  hid_t dnp_data = open_group(file_id, "/tallies/tally 1");
  hid_t neutron_data = open_group(file_id, "/tallies/tally 2");

  // check that the tallies are scoring the correct properties
  check_correct_tallies(dnp_data, neutron_data);

  // open result datasets of each tally
  hid_t dnp_dataset = open_dataset(dnp_data, "results");
  hid_t neutron_dataset = open_dataset(neutron_data, "results");

  // get the dimension size of the tally
  hsize_t dnp_dims[ndims];
  hsize_t neutron_dims[ndims];
  get_tally_shape(neutron_dataset, neutron_dims);
  get_tally_shape(dnp_dataset, dnp_dims);

  // read in tally data
  vector<double> dnp_tally_results(dnp_dims[0]);
  vector<double> neutron_tally_results(neutron_dims[0]);
  read_tally_data(dnp_dataset, dnp_dims, ndims, dnp_tally_results.data());
  read_tally_data(
    neutron_dataset, neutron_dims, ndims, neutron_tally_results.data());

  // create a vector of DNP/Neutron concentration ratios
  vector<double> dnp_neutron_ratio =
    create_dnp_neutron_ratios(dnp_tally_results, neutron_tally_results);

  // create a PDF discrete vector of neutron concentration across all mesh bins
  vector<double> neutron_pdf = create_neutron_pdf(neutron_tally_results);

  // multiply the values of the pdf by the value of the number of neutrons in
  // the time slice, then multiply this by the ratio vector
  for (int i = 0; i < neutron_pdf.size(); i++) {
    neutron_pdf[i] *= settings::num_neutrons_time_slice;
  }

  vector<double> finalized_dnps =
    calculate_normalized_precursors(dnp_neutron_ratio, neutron_pdf);

  double dnp_sum = 0.0;
  for (double value : finalized_dnps) {
    dnp_sum += value;
  }

  double ratio = (dnp_sum / settings::num_neutrons_time_slice);
  /*
  // For physics debugging purposes
  write_message(1,
    "The starting neutron population for the transient run is: {}",
    settings::num_neutrons_time_slice);
  write_message(1,
    "The sum of the normalized precursor concentrations across all families "
    "is: {}",
    dnp_sum);
  write_message(1, "The ratio of DNPs to Neutrons is therefore: {}", ratio);
  */
  // Write out the new precursor concentrations, as well as the mesh, to the
  // existing transient_source.h5 file.
  write_out_precursors(finalized_dnps);
  write_out_mesh(file_id);

  // manage HDF5 resources
  close_dataset(dnp_dataset);
  close_dataset(neutron_dataset);
  close_group(dnp_data);
  close_group(neutron_data);

  // close the statepoint file
  file_close(file_id);

  // print message indicating that the transient source was created succesfully
  write_message(
    1, "The transient_source.h5 source file was created succesfully!");
}
} // namespace openmc
