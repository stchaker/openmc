//! \file simulation.h
//! \brief Variables/functions related to running a simulation

#ifndef OPENMC_TRANSIENT_H
#define OPENMC_TRANSIENT_H

#include "openmc/particle.h"
#include "openmc/shared_array.h"
#include "mesh.h"
#include "hdf5.h"

#include <variant>

namespace openmc {

//=====================================================================
// Global variables declarations 
//=====================================================================

namespace simulation {
  extern SharedArray<SourceSite> time_slice_bank; // time-slice bank of source neutrons
  extern SharedArray<SourceSite> neutron_census; // neutron census used as bank for kinetic runs
  extern vector<double> precursor_concentrations; // vector of precursor concentrations, 6 per mesh cell
  extern std::variant<std::monostate, RectilinearMesh, SphericalMesh, CylindricalMesh> precursor_mesh; // object which functions as the precursor mesh

  // This is the maximum time taken to traverse any segment of the neutron flight 
  // path. When sampling neutrons in flight for a transient simulation to be 
  // kicked off on, they are sampled with a probability proportional to the time
  // they exist. So, this is a normalizing constant for the probability. There is 
  // a small change we encounter a probability greater than one, and in that case 
  // we round down to one.
  extern double k_t; // k-eigenvalue from k-eig run used to normalize fission source of kinetic runs
  extern double max_track_segment_time; // max track segment time used to normalize the time-slice source
  extern bool time_slice_bank_written ; // bool to determine if the time slice source was written to hdf5

} // namespace simulation

void check_tally_exists(hid_t obj_id); // function to check if the tallies exist to construct the transient source

void check_correct_tallies(hid_t obj_id_dnp, hid_t obj_id_neutron); // function to ensure existing tallies are storing the correct scores

void get_tally_shape(hid_t obj_id, hsize_t* dims); // function to get the shape of the tally data in transient_statepoint file

void read_tally_data(hid_t obj_id, hsize_t* dims, const int ndims, double* data); // function to read in the tally data from transient_statepoint file

vector<double> create_dnp_neutron_ratios(const vector<double>& dnps, const vector<double>& neutrons); // function to get dnp/neutron ratios for dmc startup.

vector<double> create_neutron_pdf(const vector<double>& neutrons); // function to create pdf of neutron values in our domain

vector<double> calculate_normalized_precursors(const vector<double>& ratios, const vector<double>& pdf); // function to create the normalized precursor concentrations for our problem

void write_out_precursors(const vector<double>& precursors); // function to write normalized precursor concentrations to the source file

void write_out_mesh(hid_t statepoint_file); // function to copy over precursor mesh from keff run to transient source file.

void write_out_keff(double keff); // function to write the simulation keff value to the transient source file.

void read_in_keff(const std::string& sourcefile); // function to read in transient normalizing keff

vector<double> read_data(const std::string& sourcefile, const std::string& attr, bool within_mesh);

void read_precursor_concentrations(const std::string& sourcefile); // function to read into memory the precursor concentrations from the transient_source.h5 file

void read_timeslice_source(const std::string& sourcefile); // function to read into memory the initial set of SourceSites for the transient run.

const std::string get_mesh_shape(const std::string& sourcefile); // function to get the mesh type from the transient_source.h5 file.

// the following three functions are used to read in the precursor mesh depending on the mesh type 
SphericalMesh get_spherical_precmesh(const std::string& sourcefile);

CylindricalMesh get_cylindrical_precmesh(const std::string& sourcefile);

RectilinearMesh get_rectilinear_precmesh(const std::string& sourcefile);

void finalize_transient_source(); // function to generate the transient_source.h5 file for use in dynamic simulation runs.

void read_transient_source(const std::string& sourcefile); // function to load in the transient_source.h5 file for use in dynamic simulation runs. 
} // namespace openmc

#endif // OPENMC_TRANSIENT_H
