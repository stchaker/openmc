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

void get_tally_shape(hid_t obj_id, hsize_t* dims); // function to get the shape of the tally data in transient_statepoint file

void read_tally_data(hid_t obj_id, hsize_t* dims, double* data); // function to read in the tally data from transient_statepoint file

vector<double> create_dnp_neutron_ratios(const vector<double>& dnps, const vector<double>& neutrons); // function to get dnp/neutron ratios for dmc startup.

vector<double> create_neutron_pdf(const vector<double>& neutrons); // function to create pdf of neutron values in our domain

const vector<double> calculate_normalized_precursors(const vector<double>& ratios, const vector<double>& pdf); // function to create the normalized precursor concentrations for our problem

void write_out_precursors(const vector<double>& precursors); // function to write normalized precursor concentrations to the source file

void write_out_mesh(hid_t statepoint_file); // function to copy over precursor mesh from keff run to transient source file. 

void finalize_transient_source(); // function to generate the transient_source.h5 file for use in dynamic simulation runs.

} // namespace openmc

#endif // OPENMC_TRANSIENT_H
