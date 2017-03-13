#include "ProbeLinkMatcher.h"


ProbeLinkMatcher::ProbeLinkMatcher(std::vector<LinkRow> &link_row_dataset_arg,
                                   std::vector<ProbeRow> &probe_row_buffer_arg) {
  link_row_dataset = link_row_dataset_arg;
  probe_row_buffer = probe_row_buffer_arg;
}

void ProbeLinkMatcher::MatchProbes() {
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

  probe_row_matched.sampleID = probe_row.sampleID;
  probe_row_matched.dateTime = probe_row.dateTime;
  probe_row_matched.sourceCode = probe_row.sourceCode;
  probe_row_matched.latitude = probe_row.latitude;
  probe_row_matched.longitude = probe_row.longitude;
  probe_row_matched.altitude = probe_row.altitude;
  probe_row_matched.speed = probe_row.speed;
  probe_row_matched.heading = probe_row.heading;

  int closest_link_idx = 0;
  float min_dist = Probe2LinkDistance(probe_row, link_row_dataset[0]);
  float curr_dist;
  int start_idx, end_idx, half_window = link_row_window / 2;

  if (prev_match_link_idx == -1) {
    start_idx = 1;
    end_idx = link_row_dataset.size();
  }
  else {
    start_idx = prev_match_link_idx - half_window;
    end_idx = prev_match_link_idx + half_window;
    start_idx = start_idx < 0 ? 1 : start_idx;
    end_idx = end_idx >= link_row_dataset.size() ? link_row_dataset.size()
      : end_idx;
  }

  for (int i = start_idx; i < end_idx; i++) {
    curr_dist = Probe2LinkDistance(probe_row, link_row_dataset[i]);
    if (curr_dist < min_dist) {
      min_dist = curr_dist;
      closest_link_idx = i;
    }
  }

  probe_row_matched.linkPVID = link_row_dataset[closest_link_idx].linkPVID;
  probe_row_matched.direction = ProbeDirectionInLink(probe_row,
                                                     link_row_dataset
                                                     [closest_link_idx]);

  return std::make_pair(probe_row_matched, closest_link_idx);
}

char ProbeLinkMatcher::ProbeDirectionInLink(ProbeRow &probe_sample,
                                            LinkRow &link) {
  ProbeLinkTriangle probe_link_triangle = ExtractTrianglePoints(probe_sample,
                                                                link);
  std::pair<float, float> L0_P_vector = GetPoint2PointVector(probe_link_triangle.L0,
                                                             probe_link_triangle.P);
  float L0_P_vector_angle = std::acosf(VectorCosine(L0_P_vector,
                                                     std::make_pair(0.0f,
                                                                    1.0f)));
  L0_P_vector_angle /= (M_PI / 180.0f);
  L0_P_vector_angle = L0_P_vector_angle > 180
    ? (L0_P_vector_angle - 360) : L0_P_vector_angle; //FIX THIS
  float point_heading_angle = std::abs(probe_sample.heading 
                                       - L0_P_vector_angle);
  char direction = point_heading_angle > 90 ? 'T' : 'F';
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
  std::pair<float, float> L0_P_vector
    = GetPoint2PointVector(probe_link_triangle.L0, probe_link_triangle.P);
  float cos_theta = VectorCosine(L0_L1_vector, L0_P_vector);
  float sin_sqr_theta = 1.0f - powf(cos_theta, 2);
  float L_cos_theta = L * cos_theta;
  float dist = sqrtf(L * L * sin_sqr_theta
                     + powf((std::min(abs(std::min(L_cos_theta, link.length)),
                                      abs(std::max(0.0f, L_cos_theta)))
                             - L_cos_theta), 2));
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