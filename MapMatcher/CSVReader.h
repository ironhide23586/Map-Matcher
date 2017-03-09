#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <time.h>
#include <iomanip>
#include <chrono>

struct LinkRow {
  unsigned int linkPVID;
  unsigned int refNodeID;
  unsigned int nrefNodeID;
  float length;
  char functionalClass;
  char directionOfTravel;
  char speedCategory;
  int fromRefSpeedLimit;
  int toRefSpeedLimit;
  char fromRefNumLanes;
  char toRefNumLanes;
  bool multiDigitized;
  bool urban;
  float timeZone;
  std::vector<std::vector<float>> shapeInfo;
  std::vector<std::vector<float>> curvatureInfo;
  std::vector<std::vector<float>> slopeInfo;
};

struct ProbeRow {
  unsigned int sampleID;
  float dateTime; //Convert incoming date string to timestamp
  char sourceCode;
  float latitude;
  float longitude;
  float altitude;
  float speed;
  float heading;
};

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
  
  void ReadNextRow();
  void PopulateReadBuffer();

  void ParseToLinkRowBuffer();
  void ParseToProbeRowBuffer();

  void CSVReader::split_(const std::string &s, char delim,
                         std::vector<std::string> &elems);
  std::vector<std::string> CSVReader::split(const std::string &s,
                                            char delim);

  LinkRow row_string_buff2LinkRow(std::vector<std::string>
                                  row_string_set);
  ProbeRow row_string_buff2ProbeRow(std::vector<std::string>
                                    row_string_set);
};

