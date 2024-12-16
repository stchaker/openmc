#include "openmc/transient.h"
#include "openmc/source.h"

namespace openmc{
namespace simulation{
SharedArray<SourceSite> time_slice_bank;
double max_track_segment_time {0.0}; 
bool time_slice_bank_written = false; 
} // namespace simulation

// function to perform normalization of precursor tallies for dmc startup. 
void finalize_dmc_source() {
  const std::string file_path = "timeslice_source.h5";
  FileSource file_source = FileSource(file_path);
  vector<SourceSite> neutron_sites = file_source.get_sites_from_file(); 
}

} // namespace openmc 
