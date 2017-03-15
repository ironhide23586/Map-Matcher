#include "ProbeLinkMatcher.h"

ProbeLinkMatcher::ProbeLinkMatcher() {
  
}

void ProbeLinkMatcher::MatchProbes(std::vector<LinkRow> &link_row_dataset_arg,
                                   std::vector<ProbeRow> &probe_row_buffer_arg) {
  link_row_dataset = link_row_dataset_arg;
  probe_row_buffer = probe_row_buffer_arg;
  matched_probe_row_buffer.clear();
  matched_probe_row_buffer.resize(probe_row_buffer.size());
  std::pair<ProbeLinkMatchRow, int> match_result;
  match_result = MatchProbe_CPU(probe_row_buffer[0]);
  matched_probe_row_buffer[0] = match_result.first;
  for (int i = 1; i < probe_row_buffer.size(); i++) {
    match_result = MatchProbe_CPU(probe_row_buffer[i], match_result.second);
    matched_probe_row_buffer[i] = match_result.first;
  }
}

std::pair<ProbeLinkMatchRow,
          int> ProbeLinkMatcher::MatchProbe_CPU(ProbeRow &probe_row,
                                                int prev_match_link_idx,
                                                int link_row_window) {
  ProbeLinkMatchRow probe_row_matched;
  link_scan_window = link_row_window;
  probe_row_matched.sampleID = probe_row.sampleID;
  probe_row_matched.dateTime = probe_row.dateTime;
  probe_row_matched.dateTime_str.assign(probe_row.dateTime_str);
  probe_row_matched.sourceCode = probe_row.sourceCode;
  probe_row_matched.latitude = probe_row.latitude;
  probe_row_matched.longitude = probe_row.longitude;
  probe_row_matched.altitude = probe_row.altitude;
  probe_row_matched.speed = probe_row.speed;
  probe_row_matched.heading = probe_row.heading;
  int closest_link_idx = 0;
  float min_dist = FLT_MAX;
  float curr_dist;
  int start_idx, end_idx, half_window = link_row_window / 2;
  std::pair<int, float> closest_idx_min_dist;
  std::pair<int, int> start_end_idx;
  if (prev_match_link_idx == -1) {
    start_idx = 0;
    //end_idx = link_row_dataset.size();
    end_idx = link_row_window;
    start_end_idx = std::make_pair(start_idx, end_idx);
  }
  else {
    start_idx = prev_match_link_idx - half_window;
    end_idx = prev_match_link_idx + half_window;
    start_end_idx = ClipStartEndIndices(start_idx, end_idx);
  }
  closest_idx_min_dist = FindClosestLinkDistIdx(probe_row, start_end_idx);
  closest_link_idx = closest_idx_min_dist.first;
  min_dist = closest_idx_min_dist.second;
  ProbeLinkTriangle min_triangle = ExtractTrianglePoints(probe_row,
                                                         link_row_dataset
                                                         [closest_link_idx]);
  float L0_P_dist = HaversineDistance(min_triangle.L0, min_triangle.P);

  //ProbeLinkTriangle min_triangle1 = ExtractTrianglePoints(probe_row,
  //                                                       link_row_dataset
  //                                                       [closest_link_idx-1]);
  //ProbeLinkTriangle min_triangle2 = ExtractTrianglePoints(probe_row,
  //                                                        link_row_dataset
  //                                                        [closest_link_idx + 1]);

  probe_row_matched.linkPVID = link_row_dataset[closest_link_idx].linkPVID;

  //float dc = Probe2LinkDistance(probe_row, link_row_dataset[closest_link_idx]);
  //float dc1 = Probe2LinkDistance(probe_row, link_row_dataset[closest_link_idx-1]);
  //float dc2 = Probe2LinkDistance(probe_row, link_row_dataset[closest_link_idx + 1]);

  probe_row_matched.direction = ProbeDirectionInLink(probe_row,
                                                     link_row_dataset
                                                     [closest_link_idx]);
  //float dc11 = Probe2LinkDistance(probe_row, link_row_dataset[closest_link_idx - 1]);
  probe_row_matched.distFromRef = sqrtf(powf(L0_P_dist, 2) - powf(min_dist, 2));
  probe_row_matched.distFromLink = min_dist;
  return std::make_pair(probe_row_matched, closest_link_idx);
}

std::pair<int, float> ProbeLinkMatcher::FindClosestLinkDistIdx(ProbeRow &probe_row,
                                                               std::pair<int, int>
                                                               start_end_idx,
                                                               float thres) {
  int start_idx = start_end_idx.first, end_idx = start_end_idx.second;
  if (end_idx <= start_idx) {
    return std::make_pair(-1, -1);
  }
  std::pair<int, float> ans;
  std::pair<int, int> start_end_idx_local;
  int closest_link_idx = 0;
  float min_dist = FLT_MAX;
  float curr_dist;
  for (int i = start_idx; i < end_idx; i++) {
    curr_dist = Probe2LinkDistance(probe_row, link_row_dataset[i]);
    if (curr_dist < min_dist) {
      min_dist = curr_dist;
      closest_link_idx = i;
    }
  }
  int start_idx_local, end_idx_local, step = 1;
  bool upper_done = false, lower_done = false;
  while (min_dist > thres) {
    start_idx_local = start_idx - (step * link_scan_window);
    end_idx_local = start_idx - ((step - 1) * link_scan_window);
    start_end_idx_local = ClipStartEndIndices(start_idx_local,
                                              end_idx_local);
    start_idx_local = start_end_idx_local.first;
    end_idx_local = start_end_idx_local.second;
    upper_done = end_idx_local == 0 ? true : false;
    for (int i = start_idx_local; i < end_idx_local; i++) {
      curr_dist = Probe2LinkDistance(probe_row, link_row_dataset[i]);
      if (curr_dist < min_dist) {
        min_dist = curr_dist;
        closest_link_idx = i;
      }
    }
    if (min_dist < thres)
      break;
    start_idx_local = end_idx + ((step - 1) * link_scan_window);
    end_idx_local = end_idx + (step * link_scan_window);
    start_end_idx_local = ClipStartEndIndices(start_idx_local,
                                              end_idx_local);
    start_idx_local = start_end_idx_local.first;
    end_idx_local = start_end_idx_local.second;
    lower_done = end_idx_local == link_row_dataset.size() ? true : false;
    for (int i = start_idx_local; i < end_idx_local; i++) {
      curr_dist = Probe2LinkDistance(probe_row, link_row_dataset[i]);
      if (curr_dist < min_dist) {
        min_dist = curr_dist;
        closest_link_idx = i;
      }
    }
    if (upper_done && lower_done)
      break;
    step++;
  }
  ans = std::make_pair(closest_link_idx, min_dist);
  return ans;
}

std::pair<int, int> ProbeLinkMatcher::ClipStartEndIndices(int start_idx, int end_idx) {
  int start_idx_clipped = start_idx, end_idx_clipped = end_idx;
  if (start_idx < 0)
    start_idx_clipped = 0;
  else if (start_idx_clipped >= link_row_dataset.size())
    start_idx_clipped = link_row_dataset.size() - 1;
  if (end_idx < 0)
    end_idx_clipped = 0;
  else if (end_idx >= link_row_dataset.size())
    end_idx_clipped = link_row_dataset.size();
  return std::make_pair(start_idx_clipped, end_idx_clipped);
}

char ProbeLinkMatcher::ProbeDirectionInLink(ProbeRow &probe_sample,
                                            LinkRow &link) {
  ProbeLinkTriangle probe_link_triangle = ExtractTrianglePoints(probe_sample,
                                                                link);
  char direction = 'F';
  if (link.directionOfTravel != 'B')
    direction = link.directionOfTravel;
  else {
    std::pair<float, float> L0_P_vector = GetPoint2PointVector(probe_link_triangle.L0,
                                                               probe_link_triangle.P);
    float L0_P_vector_angle = std::acosf(VectorCosine(L0_P_vector,
                                                      std::make_pair(0.0f,
                                                                     1.0f)));
    L0_P_vector_angle /= (M_PI / 180.0f);
    float point_heading_angle = std::abs(probe_sample.heading
                                         - L0_P_vector_angle);
    if (point_heading_angle > 90 && point_heading_angle <= 270)
      direction = 'T';
  }
  return direction;
}

std::pair<float, float> ProbeLinkMatcher::GetPoint2PointVector(
  std::pair<float, float> &p0, std::pair<float, float> &p1) {
  return std::make_pair(p1.first - p0.first, p1.second - p0.second);
}

ProbeLinkTriangle ProbeLinkMatcher::ExtractTrianglePoints(ProbeRow &probe_sample,
                                                          LinkRow &link) {
  ProbeLinkTriangle triangle;
  triangle.L0 = std::make_pair(link.shapeInfo[0][0],
                               link.shapeInfo[0][1]);
  triangle.L1 = std::make_pair(link.shapeInfo.back()[0],
                               link.shapeInfo.back()[1]);
  triangle.P = std::make_pair(probe_sample.latitude,
                              probe_sample.longitude);
  return triangle;
}

float ProbeLinkMatcher::deg2rad(float deg) {
  return deg * M_PI / 180;
}

float ProbeLinkMatcher::VectorCosine(std::pair<float, float> &v0,
                                     std::pair<float, float> &v1) {
  float numerator = v0.first * v1.first + v0.second * v1.second;
  float denominator = sqrtf(powf(v0.first, 2) + powf(v0.second, 2))
    * sqrtf(powf(v1.first, 2) + powf(v1.second, 2));
  return numerator / denominator;
}

float ProbeLinkMatcher::Probe2LinkDistance(ProbeRow &probe_sample,
                                           LinkRow &link) {
  ProbeLinkTriangle probe_link_triangle = ExtractTrianglePoints(probe_sample,
                                                                link);
  float L = HaversineDistance(probe_link_triangle.P, probe_link_triangle.L0);
  std::pair<float, float> L0_L1_vector
    = GetPoint2PointVector(probe_link_triangle.L0, probe_link_triangle.L1);
  if (L0_L1_vector.first == 0 && L0_L1_vector.second == 0)
    return FLT_MAX;
  std::pair<float, float> L0_P_vector
    = GetPoint2PointVector(probe_link_triangle.L0, probe_link_triangle.P);
  float cos_theta = VectorCosine(L0_L1_vector, L0_P_vector);
  float sin_sqr_theta = 1.0f - powf(cos_theta, 2);
  float L_cos_theta = L * cos_theta;
  
  float dist = sqrtf(L * L * sin_sqr_theta
                     + powf((std::min(abs(std::min(L_cos_theta, link.length)),
                                      abs(std::max(0.0f, L_cos_theta)))
                             - L_cos_theta), 2));
  if (!isfinite(dist)) {
    return 0.0f;
  }
  return dist;
}

float ProbeLinkMatcher::HaversineDistance(std::pair<float, float> &p0,
                                          std::pair<float, float> &p1) {
  float lat0_rad = deg2rad(p0.first), long0_rad = deg2rad(p0.second);
  float lat1_rad = deg2rad(p1.first), long1_rad = deg2rad(p1.second);
  float a = pow(sinf((lat1_rad - lat0_rad) / 2.0f), 2)
    + cosf(lat1_rad) * cosf(lat0_rad)
    * pow(sinf((long1_rad - long0_rad) / 2.0f), 2);
  float c = 2 * atan2f(sqrtf(a), sqrtf(1.0f - a));
  return 6371000.0f * c; //Earth's radius in metres
}