#include "MapMatcherCommonHeader.h"

class ProbeLinkMatcher {
public:
  ProbeLinkMatcher();

  std::vector<LinkRow> link_row_dataset;
  std::vector<ProbeRow> probe_row_buffer;
  std::vector<ProbeLinkMatchRow> matched_probe_row_buffer;
  int link_scan_window;

  void MatchProbes(std::vector<LinkRow> &link_row_dataset_arg,
                   std::vector<ProbeRow> &probe_row_buffer_arg);

  std::pair<ProbeLinkMatchRow, int> MatchProbe_CPU(ProbeRow &probe_row,
                                                   int prev_match_link_idx = -1,
                                                   int link_row_window = 100);
  float static deg2rad(float deg);
  float Probe2LinkDistance(ProbeRow &probe_sample, LinkRow &link);
  char ProbeDirectionInLink(ProbeRow &probe_sample, LinkRow &link);
  float HaversineDistance(std::pair<float, float> &p0,
                          std::pair<float, float> &p1);
  std::pair<float, float> GetPoint2PointVector(std::pair<float, float> &p0,
                                               std::pair<float, float> &p1);
  float VectorCosine(std::pair<float, float> &v0, std::pair<float, float> &v1);
  ProbeLinkTriangle ExtractTrianglePoints(ProbeRow &probe_sample,
                                          LinkRow &link);
  std::pair<int, int> ClipStartEndIndices(int start_idx, int end_idx);
  std::pair<int, float> FindClosestLinkDistIdx(ProbeRow &probe_row,
                                               std::pair<int, int> start_end_idx,
                                               float thres = 30.0f);
};

