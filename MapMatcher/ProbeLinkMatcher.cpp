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
    start_idx = start_idx < 0 ? 0 : start_idx;
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
  char dir;

  return dir;
}

float ProbeLinkMatcher::deg2rad(float deg) {
  return deg * M_PI / 180;
}

float ProbeLinkMatcher::Probe2LinkDistance(ProbeRow &probe_sample,
                                           LinkRow &link) {
  float dist = 0;
  float D = link.length;
  std::pair<float, float> L0 = std::make_pair(link.shapeInfo[0][0],
                                              link.shapeInfo[0][1]);
  std::pair<float, float> L1 = std::make_pair(link.shapeInfo.back()[0],
                                              link.shapeInfo.back()[1]);
  std::pair<float, float> P = std::make_pair(probe_sample.latitude,
                                             probe_sample.longitude);
  float L = HaversineDistance(P, L0);
  std::pair<float, float> L0_L1_vector = std::make_pair(L1.first - L0.first,
                                                        L1.second - L0.second);
  std::pair<float, float> L0_P_vector = std::make_pair(P.first - L0.first,
                                                       P.second - L0.second);
  float numerator = L0_L1_vector.first * L0_P_vector.first
    + L0_L1_vector.second * L0_P_vector.second;
  float denominator = sqrt(powf(L0_L1_vector.first, 2) + powf(L0_L1_vector.second, 2))
    * sqrt(powf(L0_P_vector.first, 2) + powf(L0_P_vector.second, 2));
  
  float cos_theta = numerator / denominator;
  float sin_sqr_theta = 1.0f - powf(cos_theta, 2);
  float L_cos_theta = L * cos_theta;

  dist = sqrtf(L * L * sin_sqr_theta
               + powf((std::min(abs(std::min(L_cos_theta, D)),
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