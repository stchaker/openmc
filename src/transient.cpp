#include "openmc/transient.h"
#include "openmc/source.h"
#include "openmc/hdf5_interface.h"
#include "hdf5.h"

#include<iostream>
namespace openmc{
namespace simulation{
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {0.0}; 
bool time_slice_bank_written = false; 
} // namespace simulation

// returns the size of the timeslice bank written out during the simulation
const size_t calculate_timeslice_size() {
  const std::string file_path = "timeslice_source.h5";
  FileSource file_source = FileSource(file_path);
  vector<SourceSite> neutron_sites = file_source.get_sites_from_file();
  const size_t timeslice_bank_size = neutron_sites.size();  
  return timeslice_bank_size; 
}

// returns the size of the 1D array tallies
void get_tally_shape(hid_t obj_id, hsize_t* dims) {
  hid_t d_space;
  d_space = H5Dget_space(obj_id); 
  H5Sget_simple_extent_dims(d_space, dims, nullptr);
  H5Sclose(d_space);  
}

// reads tally results to a buffer of data 
void read_tally_data(hid_t obj_id, hsize_t* dims, const int ndims, double* data) {
  // set the dataspace and hyperslab parameters
  hid_t d_space = H5Dget_space(obj_id);
  hsize_t start[3], count[3], stride[3], block[3];

  //allocate hyperslab components
  for(int i=0; i<ndims; i++){
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

vector<double> create_dnp_neutron_ratios(const vector<double>& dnps, const vector<double>& neutrons) {
  vector<double> ratios(dnps.size());
  int neutron_counter = 0;
  const int num_delayed_groups = 6;
  for(int i=0; i<dnps.size(); i++){
    if(i%num_delayed_groups == 0 and i!= 0){
      neutron_counter++;
    }
    ratios[i] = dnps[i] / neutrons[neutron_counter];
  }
  return ratios; 
}

// routine to create the dmc_startup.h5 file needed for dmc simulation
void finalize_dmc_source(){
  // constant expressions
  constexpr int ndims = 3;

  // obtain time_slice bank size
  const size_t timeslice_size = calculate_timeslice_size();

  // open dmc_statepoint file
  const std::string filename = "dmc_statepoint.h5";
  hid_t file_id = file_open(filename, 'r', false);

  //get precursor and neutron group handles
  hid_t dnp_data = open_group(file_id, "/tallies/tally 1");
  hid_t neutron_data = open_group(file_id, "/tallies/tally 2");

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
  read_tally_data(neutron_dataset, neutron_dims, ndims, neutron_tally_results.data());

  //create a vector of DNP/Neutron concentration ratios
  vector<double> dnp_neutron_ratio = create_dnp_neutron_ratios(dnp_tally_results, neutron_tally_results);

  //create a PDF discrete vector of neutron concentration across all mesh bins
  

  // manage resources
  close_dataset(dnp_dataset);
  close_dataset(neutron_dataset);
  close_group(dnp_data);
  close_group(neutron_data);

  // close the file
  file_close(file_id); 
}
} // namespace openmc 
