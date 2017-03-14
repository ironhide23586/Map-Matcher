#include "CSVWriter.h"

CSVWriter::CSVWriter(const char *filename_arg) {
  filename = filename_arg;
  csv_fstream.open(filename);
}

void CSVWriter::WriteResults(std::vector<ProbeLinkMatchRow> &matched_probes) {
  matched_probes_dataset = matched_probes;
  if (!csv_fstream.is_open()) {
    csv_fstream.open(filename, std::ios::app);
  }
  for (auto matched_probe : matched_probes_dataset) {
    std::string row_str = ConstructCSVRowString(matched_probe);
    csv_fstream << row_str;
  }
  csv_fstream.close();
}

std::string CSVWriter::ConstructCSVRowString(ProbeLinkMatchRow &probe_data) {
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