#include "CSVWriter.h"

CSVWriter::CSVWriter(const char *filename_arg) {
  filename = filename_arg;
  csv_fstream.open(filename);
}

void CSVWriter::WriteMatchedProbeResults(std::vector<ProbeLinkMatchRow>
                                         &matched_probes) {
  matched_probes_dataset = matched_probes;
  if (!csv_fstream.is_open()) {
    csv_fstream.open(filename, std::ios::app);
  }
  for (auto matched_probe : matched_probes_dataset) {
    std::string row_str = ConstructMatchedProbeCSVRowString(matched_probe);
    csv_fstream << row_str;
  }
  csv_fstream.close();
}

void CSVWriter::WriteLinkSlopeResults(std::unordered_map<int, float>
                                      &link_slopes) {
  std::string row_str;
  if (!csv_fstream.is_open()) {
    csv_fstream.open(filename, std::ios::app);
  }
  for (auto slope_iter = link_slopes.begin(); slope_iter != link_slopes.end();
       slope_iter++) {
    row_str.clear();
    row_str += std::to_string(slope_iter->first) + ",";
    row_str += std::to_string(slope_iter->second) + "\n";
    csv_fstream << row_str;
  }
  csv_fstream.close();
}

void CSVWriter::WriteSlopeComparisonResults(std::unordered_map
                                            <int, SlopeCompare>
                                            &link_probe_comparisons) {
  std::string row_str;
  if (!csv_fstream.is_open()) {
    csv_fstream.open(filename, std::ios::app);
  }
  for (auto slope_iter = link_probe_comparisons.begin();
       slope_iter != link_probe_comparisons.end();
       slope_iter++) {
    row_str.clear();
    row_str += std::to_string(slope_iter->first) + ",";
    row_str += std::to_string(slope_iter->second.link_slope) + ",";
    row_str += std::to_string(slope_iter->second.probe_slope) + ",";
    row_str += std::to_string(slope_iter->second.slope_diff) + "\n";
    csv_fstream << row_str;
  }
  csv_fstream.close();
}

std::string CSVWriter::ConstructMatchedProbeCSVRowString(ProbeLinkMatchRow
                                                         &probe_data) {
  std::string row_str;
  row_str.append(std::to_string(probe_data.sampleID));
  row_str.append(",");
  row_str.append(probe_data.dateTime_str);
  row_str.append(",");
  row_str.append(std::to_string(probe_data.sourceCode));
  row_str.append(",");
  row_str.append(std::to_string(probe_data.latitude));
  row_str.append(",");
  row_str.append(std::to_string(probe_data.longitude));
  row_str.append(",");
  row_str.append(std::to_string((int)probe_data.altitude));
  row_str.append(",");
  row_str.append(std::to_string((int)probe_data.speed));
  row_str.append(",");
  row_str.append(std::to_string((int)probe_data.heading));
  row_str.append(",");
  row_str.append(std::to_string(probe_data.linkPVID));
  row_str.append(",");
  row_str += probe_data.direction;
  row_str.append(",");
  row_str.append(std::to_string(probe_data.distFromRef));
  row_str.append(",");
  row_str.append(std::to_string(probe_data.distFromLink));
  row_str.append("\n");
  return row_str;
}

