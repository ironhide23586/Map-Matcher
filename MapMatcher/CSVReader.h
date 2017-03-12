#include "MapMatcherCommonHeader.h"

class CSVReader {
public:

  const char *filename;
  std::ifstream csv_fstream;
  std::string read_line;
  std::vector<std::string> raw_row_buffer;
  std::vector<std::vector<std::string>> row_string_buffer;
  std::vector<LinkRow> link_row_buffer;
  std::vector<ProbeRow> probe_row_buffer;

  int global_row_idx;
  int local_row_idx;
  int row_buffer_size;

  CSVReader(const char *filename_arg, int buff_size = 5);
  
  void PopulateReadBuffer();
  void ParseToLinkRowBuffer();
  void ParseToProbeRowBuffer();
  std::vector<std::string> static CSVReader::split(const std::string &s,
                                                   char delim);

private:
  void static CSVReader::split_(const std::string &s, char delim,
                                std::vector<std::string> &elems);
  void ReadNextRow();
  LinkRow row_string_buff2LinkRow(std::vector<std::string>
                                  &row_string_set);
  ProbeRow row_string_buff2ProbeRow(std::vector<std::string>
                                    &row_string_set);
};

