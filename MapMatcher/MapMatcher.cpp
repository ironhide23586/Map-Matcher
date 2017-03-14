#include <stdio.h>
#include <iostream>

#include "CSVReader.h"
#include "ProbeLinkMatcher.h"

using namespace std;

int main() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv", 1000); //200089 rows
  //CSVReader *probe_data_reader = new CSVReader("Partition6467ProbePoints.csv", 500);
  CSVReader *probe_data_reader = new CSVReader("probes_chunk.csv", 10);
  ProbeLinkMatcher *probe_matcher = new ProbeLinkMatcher();

  link_data_reader->PopulateReadBuffer(); // Takes 3 mins with full link data file
  link_data_reader->ParseToLinkRowBuffer();

  while (!probe_data_reader->end_reached) {
    probe_data_reader->PopulateReadBuffer();
    probe_data_reader->ParseToProbeRowBuffer();
    probe_matcher->MatchProbes(link_data_reader->link_row_buffer,
                               probe_data_reader->probe_row_buffer);
    int a = 90;
  }
}