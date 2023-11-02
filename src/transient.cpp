#include "openmc/transient.h"

namespace openmc {
namespace simulation {
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {0.0};
bool time_slice_bank_written = false;
} // namespace simulation
} // namespace openmc
