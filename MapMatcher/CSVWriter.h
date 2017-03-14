#include "MapMatcherCommonHeader.h"

class CSVWriter {
public:
  CSVWriter(const char *filename_arg);

  const char *filename;
  std::ofstream csv_fstream;
  std::vector<ProbeLinkMatchRow> matched_probes_dataset;

  void WriteResults(std::vector<ProbeLinkMatchRow> &matched_probes);
  std::string ConstructCSVRowString(ProbeLinkMatchRow &probe_data);
};