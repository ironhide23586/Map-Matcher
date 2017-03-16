#include "MapMatcherCommonHeader.h"

class LinkSlopeFromProbesFinder {
public:
  std::unordered_map<int, std::vector<LinkPoint>> link_details;
  std::unordered_map<int, float> link_slope_map_computed;
  std::unordered_map<int, float> link_slope_map_obtained;
  std::unordered_map<int, SlopeCompare> link_probe_slope_comparison;

  float GetSlope(std::vector<LinkPoint> &link_points);

  void ComputeSlopes(std::unordered_map<int,
                     std::vector<LinkPoint>> &link_details_arg);

  void GetSlopesFromLinkFile(std::vector<LinkRow> &links_from_file);

  void CompareLinkProbeSlopes();
};

