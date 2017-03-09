#include <stdio.h>
#include <vector>
#include <iostream>

#include "CSVReader.h"

using namespace std;

int main() {
  CSVReader *link_data_reader = new CSVReader("Partition6467LinkData.csv", 1000); //200089 rows
  CSVReader *probe_data_reader = new CSVReader("Partition6467ProbePoints.csv", 500);
  
//#pragma omp parallel
//  std::cout << "Lol" << std::endl;
//
  probe_data_reader->PopulateReadBuffer();
  probe_data_reader->ParseToProbeRowBuffer();

  link_data_reader->PopulateReadBuffer(); // Takes 3 mins with full link data file
  link_data_reader->ParseToLinkRowBuffer();

  

  int k = 0;
}