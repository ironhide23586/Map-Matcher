#include "MapMatcherCommonHeader.h"

class ProbeLinkMatcher {
public:
  ProbeLinkMatcher(std::vector<LinkRow> &link_row_dataset_arg,
                   std::vector<ProbeRow> &probe_row_buffer_arg);

  std::vector<LinkRow> link_row_dataset;
  std::vector<ProbeRow> probe_row_buffer;
  std::vector<ProbeLinkMatchRow> matched_probe_row_buffer;

  void MatchProbes();

  ProbeLinkMatchRow MatchProbe_CPU(ProbeRow &probe_row);
  float static deg2rad(float deg);
  float Probe2LinkDistance(ProbeRow &probe_sample, LinkRow &link);
  float HaversineDistance(std::pair<float, float> &p0,
                          std::pair<float, float> &p1);
};

