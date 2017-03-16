#include <stdio.h>
#include <iostream>
#include <string>

#include "CSVReader.h"
#include "CSVWriter.h"
#include "ProbeLinkMatcher.h"
#include "LinkSlopeFromProbesFinder.h"

using namespace std;

#define LINK_DATA_FILE_ROWS 2000//200089
#define PROBE_MATCH_PROCESSING_BATCH_SIZE 600

void match_probes_and_links(void);

void find_link_slopes(void);

int main() {
  //match_probes_and_links();
  find_link_slopes();

  return 0;
}

void match_probes_and_links() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv",
                                              LINK_DATA_FILE_ROWS);
  CSVReader *probe_data_reader = new CSVReader("Partition6467ProbePoints.csv",
                                               PROBE_MATCH_PROCESSING_BATCH_SIZE);
  CSVWriter *matched_probe_data_writer = new CSVWriter("Partition6467MatchedPoints.csv");
  ProbeLinkMatcher *probe_matcher = new ProbeLinkMatcher();

  std::cout << "Loading Link Data File..." << std::endl;
  link_data_reader->PopulateReadBuffer();
  std::cout << "Link Data file Loaded! Read in " << link_data_reader->read_rows
    << " rows \n" << std::endl;
  std::cout << "Parsing Link Data file..." << std::endl;
  link_data_reader->ParseToLinkRowBuffer();
  std::cout << "Link Data file parsed! Successfully parsed in "
    << link_data_reader->parsed_rows << " rows \n" << std::endl;

  std::cout << "Press Enter to begin the map-matching process!!" << std::endl;
  getchar();

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

void find_link_slopes() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv",
                                              LINK_DATA_FILE_ROWS);
  CSVReader *matched_probes_reader = new CSVReader("Partition6467MatchedPoints.csv",
                                                   PROBE_MATCH_PROCESSING_BATCH_SIZE);
  LinkSlopeFromProbesFinder *link_slope_finder = new LinkSlopeFromProbesFinder();

  link_data_reader->PopulateReadBuffer();
  link_data_reader->ParseToLinkRowBuffer();

  while (!matched_probes_reader->end_reached) {
    matched_probes_reader->PopulateReadBuffer();
    matched_probes_reader->ParseToMatchedProbesBuffer();
  }

  link_slope_finder->GetSlopesFromLinkFile(link_data_reader->link_row_buffer);
  link_slope_finder->ComputeSlopes(matched_probes_reader->matched_probes_buffer);
  link_slope_finder->CompareLinkProbeSlopes();
}