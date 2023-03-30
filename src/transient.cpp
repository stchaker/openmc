#include "openmc/transient.h"

namespace openmc {
namespace simulation {
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {1.0e100};
} // namespace simulation
} // namespace openmc
