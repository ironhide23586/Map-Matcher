#include <stdio.h>
#include <iostream>
#include <string>

#include "CSVReader.h"
#include "CSVWriter.h"
#include "ProbeLinkMatcher.h"
#include "LinkSlopeFromProbesFinder.h"

using namespace std;

#define LINK_DATA_FILE_ROWS 200089
#define PROBE_MATCH_PROCESSING_BATCH_SIZE 600

void match_probes_and_links(void);

void find_link_slopes(void);

int main() {
  std::cout << "This is a Map- matching application which maps input dataset "
    << "of GPS points collected from a Probe to known GPS coordinates of "
    << "road links corresponding to a given geographical region." << std::endl;

  std::cout << "Please select from the options below -" << std::endl;
  std::cout << "1. Map Matching - This will read in 2 files containing "
    << "Probe GPS data & Link GPS data and match them to produce the "
    << "output file Partition6467MatchedPoints.csv (lengthy process)"
    << std::endl;
  std::cout << "2. Link Slope Finding - Step 1 must be run before this one. "
    << "If results from previous step are available, this step computes link "
    << "slopes from the probe GPS data and from the provided link data "
    << "and compares the computed slopes vs the provided slopes\n"
    << std::endl;

  int choice = 0;
  while (choice != 1 && choice != 2) {
    std::cout << "Please enter a valid choice and press enter - ";
    std::cin >> choice;
  }
  
  if (choice == 1)
    match_probes_and_links();
  else if (choice == 2)
    find_link_slopes();

  getchar();
  return 0;
}

void match_probes_and_links() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv",
                                              LINK_DATA_FILE_ROWS);
  CSVReader *probe_data_reader
    = new CSVReader("Partition6467ProbePoints.csv",
                    PROBE_MATCH_PROCESSING_BATCH_SIZE);
  CSVWriter *matched_probe_data_writer 
    = new CSVWriter("Partition6467MatchedPoints.csv");
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
    matched_probe_data_writer
      ->WriteMatchedProbeResults(probe_matcher->matched_probe_row_buffer);
    std::cout << "Wrote results to file! :D" << std::endl;
    start_idx += read_rows;
    std::cout << "-----------------------" << std::endl;
  }
  std::cout << "All results written to Partition6467MatchedPoints.csv!! :D"
    << std::endl;
  getchar();
}

void find_link_slopes() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv",
                                              LINK_DATA_FILE_ROWS);
  CSVReader *matched_probes_reader
    = new CSVReader("Partition6467MatchedPoints.csv",
                    PROBE_MATCH_PROCESSING_BATCH_SIZE);
  CSVWriter *computed_slopes_writer 
    = new CSVWriter("Partition6467ComputedSlopes.csv");
  CSVWriter *link_file_slopes_writer 
    = new CSVWriter("Partition6467LinkDataSlopes.csv");
  CSVWriter *computed_vs_link_file_slopes_writer 
    = new CSVWriter("Partition6467ComputedVsLinkDataSlopeComparisons.csv");
  LinkSlopeFromProbesFinder *link_slope_finder 
    = new LinkSlopeFromProbesFinder();

  std::cout << "Loading Link Data File..." << std::endl;
  link_data_reader->PopulateReadBuffer();
  std::cout << "Link Data file Loaded! Read in " << link_data_reader->read_rows
    << " rows \n" << std::endl;
  std::cout << "Parsing Link Data file..." << std::endl;
  link_data_reader->ParseToLinkRowBuffer();
  std::cout << "Link Data file parsed! Successfully parsed in "
    << link_data_reader->parsed_rows << " rows \n" << std::endl;

  std::cout << "Computing Link Slopes from provided Link data File..." 
    << std::endl;
  link_slope_finder->GetSlopesFromLinkFile(link_data_reader->link_row_buffer);
  std::cout 
    << "Slopes Computed!, Writing results to Partition6467LinkDataSlopes.csv"
    << std::endl;
  link_file_slopes_writer->WriteLinkSlopeResults(link_slope_finder
                                                 ->link_slope_map_obtained);
  std::cout << "Wrote results to file! :D" << std::endl;

  std::cout << "\nLoading & Parsing Data from Partition6467MatchedPoints.csv..."
    << std::endl;
  int rows_parsed = 0;
  while (!matched_probes_reader->end_reached) {
    matched_probes_reader->PopulateReadBuffer();
    matched_probes_reader->ParseToMatchedProbesBuffer();
    rows_parsed += matched_probes_reader->parsed_rows;
    std::cout << rows_parsed << " Rows parsed" << std::endl;
  }
  std::cout << "Loaded! :D" << std::endl;

  std::cout << "\nComputing Slopes from measured Probe Data..." << std::endl;
  link_slope_finder
    ->ComputeSlopes(matched_probes_reader->matched_probes_buffer);
  std::cout << "Computation complete! :D" << std::endl;

  std::cout << "\nComparing Computed Slopes with actual slopes from link file.."
    << std::endl;
  link_slope_finder->CompareLinkProbeSlopes();
  std::cout << "Comparison complete!" << std::endl;

  std::cout
    << "\nWriting results of slopes computed from Probe Points to "
    << "Partition6467ComputedSlopes.csv" << std::endl;
  computed_slopes_writer->WriteLinkSlopeResults(link_slope_finder
                                                ->link_slope_map_computed);
  std::cout << "Wrote results to file! :D" << std::endl;

  std::cout
    << "\nWriting comparisons of computed vs observed link slopes to "
    << "Partition6467ComputedVsLinkDataSlopeComparisons.csv" << std::endl;
  computed_vs_link_file_slopes_writer
    ->WriteSlopeComparisonResults(link_slope_finder
                                  ->link_probe_slope_comparison);
  std::cout << "Wrote results to file! :D" << std::endl;
}