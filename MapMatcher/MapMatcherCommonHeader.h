#pragma once

#define _USE_MATH_DEFINES

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <time.h>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <cfloat>
#include <unordered_map>

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
  std::string dateTime_str;
  char sourceCode;
  float latitude;
  float longitude;
  float altitude;
  float speed;
  float heading;
};

struct ProbeLinkMatchRow {
  unsigned int sampleID;
  float dateTime; //Convert incoming date string to timestamp
  std::string dateTime_str;
  char sourceCode;
  float latitude;
  float longitude;
  float altitude; // int
  float speed; // int
  float heading; // int
  unsigned int linkPVID;
  char direction;
  float distFromRef;
  float distFromLink;
};

struct ProbeLinkTriangle {
  std::pair<float, float> L0; //Reference node
  std::pair<float, float> L1; //Non-reference node
  std::pair<float, float> P; //Probe Point
};

struct LinkPoint {
  float altitude;
  float distFromRef;
  float distFromLink;
};

struct SlopeCompare {
  float link_slope;
  float probe_slope;
  float slope_diff;
};