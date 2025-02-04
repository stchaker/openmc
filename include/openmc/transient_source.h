//! \file simulation.h
//! \brief Variables/functions related to running a simulation

#ifndef OPENMC_TRANSIENT_H
#define OPENMC_TRANSIENT_H

#include "openmc/particle.h"
#include "openmc/shared_array.h"
#include "hdf5.h"

namespace openmc {

//=====================================================================
// Global variables declarations 
//=====================================================================

namespace simulation {
  extern SharedArray<SourceSite> time_slice_bank;

  // This is the maximum time taken to traverse any segment of the neutron flight 
  // path. When sampling neutrons in flight for a transient simulation to be 
  // kicked off on, they are sampled with a probability proportional to the time
  // they exist. So, this is a normalizing constant for the probability. There is 
  // a small change we encounter a probability greater than one, and in that case 
  // we round down to one.
  extern double max_track_segment_time;
  extern bool time_slice_bank_written ;

} // namespace simulation
void check_tally_exists(hid_t obj_id); // function to check if the tallies exist to construct the transient source
void check_correct_tallies(hid_t obj_id_dnp, hid_t obj_id_neutron); // function to ensure existing tallies are storing the correct scores
const size_t calculate_timeslice_size(); // function to find size of time-slice bank from hdf5 file
void get_tally_shape(hid_t obj_id, hsize_t* dims); // function to get the shape of the tally data in dmc.statepoint.h5 file
void read_tally_data(hid_t obj_id, hsize_t* dims, double* data); // function to read in the tally data from dmc.statepoint.h5 file
vector<double> create_dnp_neutron_ratios(const vector<double>& dnps, const vector<double>& neutrons); // function to get dnp/neutron ratios for dmc startup.
void finalize_dmc_source(); // function to call to generate the normalized precursor concentrations in the dmc.statepoint.h5 file 
} // namespace openmc

#endif // OPENMC_TRANSIENT_H
