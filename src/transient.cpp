#include "openmc/transient.h"
#include "openmc/source.h"

namespace openmc{
namespace simulation{
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {0.0}; 
bool time_slice_bank_written = false; 
} // namespace simulation

// Returns the size of the timeslice bank written out during the simulation
const size_t calculate_timeslice_size() {
  const std::string file_path = "timeslice_source.h5";
  FileSource file_source = FileSource(file_path);
  vector<SourceSite> neutron_sites = file_source.get_sites_from_file();
  const size_t timeslice_bank_size = neutron_sites.size();  
  return timeslice_bank_size; 
}

void finalize_dmc_source(){
  const size_t timeslice_size = calculate_timeslice_size();

}
} // namespace openmc 
