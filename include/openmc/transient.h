//! \file simulation.h
//! \brief Variables/functions related to a running simulation

#ifndef OPENMC_TRANSIENT_H
#define OPENMC_TRANSIENT_H

#include "openmc/particle.h"
#include "openmc/shared_array.h"

namespace openmc {

//==============================================================================
// Global variable declarations
//==============================================================================

namespace simulation {

extern SharedArray<SourceSite> time_slice_bank;

// This is the maximum time taken to traverse any segment of the neutron flight
// path. When sampling neutrons in flight for a transient simulation to be
// kicked off on, they are sampled with a probability proportional to the time
// they exist. So, this is a normalizing constant for that probability. There is
// a small chance we encounter a probability greater than one, and in that case
// we round down to one.
extern double max_track_segment_time;

} // namespace simulation

} // namespace openmc

#endif // OPENMC_TRANSIENT_H
