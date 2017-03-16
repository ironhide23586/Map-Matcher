#include "MapMatcherCommonHeader.h"

class CSVWriter {
public:
  CSVWriter(const char *filename_arg);

  const char *filename;
  std::ofstream csv_fstream;
  std::vector<ProbeLinkMatchRow> matched_probes_dataset;

  void WriteMatchedProbeResults(std::vector<ProbeLinkMatchRow> &matched_probes);
  void WriteLinkSlopeResults(std::unordered_map<int, float> &link_slopes);
  
  //linkPVID, link_slope, probe_slope, difference
  void WriteSlopeComparisonResults(std::unordered_map<int, SlopeCompare>
                                   &link_probe_comparisons);
  std::string ConstructMatchedProbeCSVRowString(ProbeLinkMatchRow &probe_data);
};