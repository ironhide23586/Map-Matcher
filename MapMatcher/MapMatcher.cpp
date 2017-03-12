#include <stdio.h>
//#include <vector>
#include <iostream>

#include "CSVReader.h"
#include "ProbeLinkMatcher.h"

using namespace std;

int main() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv", 1000); //200089 rows
  CSVReader *probe_data_reader = new CSVReader("Partition6467ProbePoints.csv", 500);
  
//#pragma omp parallel
//  std::cout << "Lol" << std::endl;
//

  link_data_reader->PopulateReadBuffer(); // Takes 3 mins with full link data file
  link_data_reader->ParseToLinkRowBuffer();

  probe_data_reader->PopulateReadBuffer();
  probe_data_reader->ParseToProbeRowBuffer();


  ProbeLinkMatcher *probe_matcher = new ProbeLinkMatcher(link_data_reader->link_row_buffer,
                                                         probe_data_reader->probe_row_buffer);

  probe_matcher->MatchProbes();

  int k = 0;
}