#include "CSVReader.h"

extern "C" char* strptime(const char* s,
                          const char* f,
                          struct tm* tm) {
  std::istringstream input(s);
  input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
  input >> std::get_time(tm, f);
  if (input.fail()) {
    return nullptr;
  }
  return (char*)(s + input.tellg());
}

CSVReader::CSVReader(const char *filename_arg, int buff_size) { 
  filename = filename_arg;
  csv_fstream.open(filename);
  global_row_idx = 0;
  local_row_idx = 0;
  row_buffer_size = buff_size;
  end_reached = false;
}

bool CSVReader::ReadNextRow() {
  if (csv_fstream.is_open()) {
    if (std::getline(csv_fstream, read_line)) {
      if (local_row_idx >= row_buffer_size) {
        local_row_idx = 0;
      }
      raw_row_buffer[local_row_idx] = read_line;
      row_string_buffer[local_row_idx] = split(read_line, ',');
      local_row_idx++;
      global_row_idx++;
      return true;
    }
    else {
      csv_fstream.close();
      return false;
    }
  }
  return false;
}

LinkRow CSVReader::row_string_buff2LinkRow(std::vector<std::string>
                                           &row_string_set) {
  LinkRow row;
  row.linkPVID = stoi(row_string_set[0]);
  row.refNodeID = stoi(row_string_set[1]);
  row.nrefNodeID = stoi(row_string_set[2]);
  row.length = stod(row_string_set[3]);
  row.functionalClass = row_string_set[4][0];
  row.directionOfTravel = row_string_set[5][0];
  row.speedCategory = row_string_set[6][0];
  row.fromRefSpeedLimit = stoi(row_string_set[7]);
  row.toRefSpeedLimit = stoi(row_string_set[8]);
  row.fromRefNumLanes = row_string_set[9][0];
  row.toRefNumLanes = row_string_set[10][0];
  row.multiDigitized = false;
  if (row_string_set[11][0] == 'T')
    row.multiDigitized = true;
  row.urban = false;
  if (row_string_set[12][0] == 'T')
    row.urban = true;
  row.timeZone = stod(row_string_set[13]);
  std::vector<std::string> points = split(row_string_set[14], '|');
  std::vector<float> curr_point;
  std::vector<std::string> curr_point_string;
  for (auto link_point : points) {
    curr_point_string = split(link_point, '/');
    for (auto lla : curr_point_string) {
      curr_point.push_back(stod(lla));
    }
    row.shapeInfo.push_back(curr_point);
    curr_point.clear();
    curr_point_string.clear();
  }
  points.clear();
  if (row_string_set.size() >= 16) {
    points = split(row_string_set[15], '|');
    for (auto curvature : points) {
      curr_point_string = split(curvature, '/');
      for (auto dist_curv : curr_point_string) {
        curr_point.push_back(stod(dist_curv));
      }
      row.curvatureInfo.push_back(curr_point);
      curr_point.clear();
      curr_point_string.clear();
    }
    points.clear();
  }
  if (row_string_set.size() >= 17) {
    points = split(row_string_set[16], '|');
    for (auto curvature : points) {
      curr_point_string = split(curvature, '/');
      for (auto dist_slope : curr_point_string) {
        curr_point.push_back(stod(dist_slope));
      }
      row.slopeInfo.push_back(curr_point);
      curr_point.clear();
      curr_point_string.clear();
    }
    points.clear();
  }
  return row;
}

ProbeRow CSVReader::row_string_buff2ProbeRow(std::vector<std::string>
                                             &row_string_set) {
  ProbeRow row;
  tm probe_time;
  time_t timestamp;
  row.sampleID = stoi(row_string_set[0]);
  if (strptime(row_string_set[1].c_str(), "%m/%d/%Y %r", &probe_time) != NULL)
    row.dateTime = mktime(&probe_time);
  row.dateTime_str.assign(row_string_set[1]);
  row.sourceCode = (char)stoi(row_string_set[2]);
  row.latitude = stod(row_string_set[3]);
  row.longitude = stod(row_string_set[4]);
  row.altitude = stod(row_string_set[5]);
  row.speed = stod(row_string_set[6]);
  row.heading = stod(row_string_set[7]);
  return row;
}

void CSVReader::PopulateReadBuffer() {
  raw_row_buffer.clear();
  row_string_buffer.clear();
  raw_row_buffer.resize(row_buffer_size);
  row_string_buffer.resize(row_buffer_size);
  for (int i = 0; i < row_buffer_size; i++) {
    if (!ReadNextRow()) {
      row_buffer_size = i;
      raw_row_buffer.resize(row_buffer_size);
      row_string_buffer.resize(row_buffer_size);
      end_reached = true;
      break;
    }
  }
}

void CSVReader::ParseToLinkRowBuffer() {
  link_row_buffer.clear();
  link_row_buffer.resize(row_buffer_size);
#pragma omp parallel
  for (int i = 0; i < row_buffer_size; i++) {
    link_row_buffer[i] = row_string_buff2LinkRow(row_string_buffer[i]);
  }
}

void CSVReader::ParseToProbeRowBuffer() {
  probe_row_buffer.clear();
  probe_row_buffer.resize(row_buffer_size);
  for (int i = 0; i < row_buffer_size; i++) {
    probe_row_buffer[i] = row_string_buff2ProbeRow(row_string_buffer[i]);
  }
}

void CSVReader::split_(const std::string &s, char delim,
                       std::vector<std::string> &elems) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

std::vector<std::string> CSVReader::split(const std::string &s,
                                           char delim) {
  std::vector<std::string> elems;
  split_(s, delim, elems);
  return elems;
}