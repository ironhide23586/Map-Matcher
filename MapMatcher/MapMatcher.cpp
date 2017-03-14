#include <stdio.h>
#include <iostream>

#include "CSVReader.h"
#include "CSVWriter.h"
#include "ProbeLinkMatcher.h"

using namespace std;

#define LINK_DATA_FILE_ROWS 200089
#define PROBE_MATCH_PROCESSING_BATCH_SIZE 600

int main() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv",
                                              LINK_DATA_FILE_ROWS);
  CSVReader *probe_data_reader = new CSVReader("Partition6467ProbePoints.csv",
                                               PROBE_MATCH_PROCESSING_BATCH_SIZE);
  CSVWriter *matched_probe_data_writer = new CSVWriter("Partition6467MatchedPoints.csv");
  
  std::cout << "Loading Link Data File..." << std::endl;
  link_data_reader->PopulateReadBuffer(); // Takes 3 mins with full link data file
  std::cout << "Link Data file Loaded!\n" << std::endl;
  std::cout << "Parsing Link Data file..." << std::endl;
  link_data_reader->ParseToLinkRowBuffer();
  std::cout << "Link Data file parsed!\n" << std::endl;
  ProbeLinkMatcher *probe_matcher = new ProbeLinkMatcher();
  int matched_probes = 0, start_idx = 0, read_rows, end_idx = 0;
  std::cout << "Beginning map-matching process..." << std::endl;
  while (!probe_data_reader->end_reached) {
    std::cout << "-----------------------" << std::endl; 
    probe_data_reader->PopulateReadBuffer();
    read_rows = probe_data_reader->row_buffer_size;
    end_idx = start_idx + read_rows - 1;
    std::cout << "Loaded probe points from index " << start_idx << " to "
      << end_idx << std::endl;
    probe_data_reader->ParseToProbeRowBuffer();
    std::cout << "Parsed probe points from index " << start_idx << " to "
      << end_idx << std::endl;
    probe_matcher->MatchProbes(link_data_reader->link_row_buffer,
                               probe_data_reader->probe_row_buffer);
    std::cout << "Matched probe points from index " << start_idx << " to "
      << end_idx << std::endl;
    matched_probe_data_writer->WriteResults(probe_matcher->matched_probe_row_buffer);
    std::cout << "Wrote results to file! :D" << std::endl;
    start_idx += read_rows;
    std::cout << "-----------------------" << std::endl;
  }
  std::cout << "All results written to Partition6467MatchedPoints.csv!! :D" << std::endl;
  getchar();
}