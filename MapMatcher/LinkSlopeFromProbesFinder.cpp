#include "LinkSlopeFromProbesFinder.h"

void LinkSlopeFromProbesFinder::ComputeSlopes(std::unordered_map<int,
                                              std::vector<LinkPoint>>
                                              &link_details_arg) {
  link_details = link_details_arg;
  int linkPVID;
  float link_slope;
  for (auto link_iter = link_details.begin(); link_iter != link_details.end();
       link_iter++) {
    if (link_iter->second.size() <= 1)
      continue;
    linkPVID = link_iter->first;
    link_slope = GetSlope(link_iter->second);
    link_slope_map_computed[linkPVID] = link_slope;
  }
}

float LinkSlopeFromProbesFinder::GetSlope(std::vector<LinkPoint> &link_points) {
  float slope = 0.0f;
  int min_ref_dist_idx, max_ref_dist_idx, i = 0;
  float min_ref_dist = FLT_MAX, max_ref_dist = 0.0f;
  for (auto link_point : link_points) {
    if (link_point.distFromRef < min_ref_dist) {
      min_ref_dist = link_point.distFromRef;
      min_ref_dist_idx = i;
    }
    if (link_point.distFromRef > max_ref_dist) {
      max_ref_dist = link_point.distFromRef;
      max_ref_dist_idx = i;
    }
    i++;
  }
  float triangle_base = max_ref_dist - min_ref_dist;
  float triangle_height = link_points[max_ref_dist_idx].altitude
    - link_points[min_ref_dist_idx].altitude;
  slope = std::atanf(triangle_height / triangle_base) * 180 / M_PI;
  if (!std::isfinite(slope))
    return 0.0f;
  return slope;
}

void LinkSlopeFromProbesFinder::GetSlopesFromLinkFile(std::vector<LinkRow>
                                                      &links_from_file) {
  float mean_slope;
  int num_points;
  for (auto link : links_from_file) {
    if (link.slopeInfo.size() <= 0)
      continue;
    mean_slope = 0.0f;
    num_points = 0;
    for (auto slope_tuple : link.slopeInfo) {
      mean_slope += slope_tuple[1];
      num_points++;
    }
    mean_slope /= num_points;
    link_slope_map_obtained[link.linkPVID] = mean_slope;
  }
}

void LinkSlopeFromProbesFinder::CompareLinkProbeSlopes() {
  int linkPVID;
  SlopeCompare slope_compare;
  for (auto comp_it = link_slope_map_computed.begin();
       comp_it != link_slope_map_computed.end(); comp_it++) {
    linkPVID = comp_it->first;
    auto obt_it = link_slope_map_obtained.find(linkPVID);
    if (obt_it == link_slope_map_obtained.end())
      continue;
    slope_compare.link_slope = obt_it->second;
    slope_compare.probe_slope = comp_it->second;
    slope_compare.slope_diff = std::abs(obt_it->second - comp_it->second);
    link_probe_slope_comparison[linkPVID] = slope_compare;
  }

  for (auto obt_it = link_slope_map_obtained.begin();
       obt_it != link_slope_map_obtained.end(); obt_it++) {
    linkPVID = obt_it->first;
    if (link_probe_slope_comparison.find(linkPVID)
        != link_probe_slope_comparison.end())
      continue;
    auto comp_it = link_slope_map_computed.find(linkPVID);
    if (comp_it == link_slope_map_computed.end())
      continue;
    slope_compare.link_slope = obt_it->second;
    slope_compare.probe_slope = comp_it->second;
    slope_compare.slope_diff = std::abs(obt_it->second - comp_it->second);
    link_probe_slope_comparison[linkPVID] = slope_compare;
  }
}