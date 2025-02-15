#include "resultInterpreter.h"

resultInterpreter::resultInterpreter(scanResults *res) { this->results = res; }

bool resultInterpreter::isFound(std::string resultEntry) {

  std::unordered_map<std::string, bool> &map = results->foundMap;
  auto it = map.find(resultEntry);
  if (it != map.end()) {
    if (it->second == true) {
      return true;
    }
  }
  return false;
}

void resultInterpreter::printResults() {
  std::unordered_map<std::string, bool> &map = results->foundMap;

  std::cout << "\n";
  std::cout << std::left << std::setw(30) << "Requried File" << " | "
            << std::setw(10) << " Result " << " | " << "Path \n ";
  std::cout << std::string(70, '-') << "\n"; // Separator line

  std::string resultOutput;
  std::string RequriedFile;
  std::string path = "/Example/mock";

  for (const auto &pair : map) {

    if (pair.second == false) {
      resultOutput = "RED";
      RequriedFile = pair.first + " not found ";
    }
    if (pair.second == true) {
      resultOutput = "GREEN";
      RequriedFile = pair.first + " found ";
    }

    std::cout << std::left << std::setw(30) << RequriedFile << " | "
              << std::setw(10) << std::boolalpha << resultOutput << " | "
              << path << "\n";
  }
}

void resultInterpreter::printDetails() {

  // Header
  std::cout << "\n";
  std::cout << std::setw(70) << "Details";
  std::cout << std::string(70, '-');
  std::cout << "\n";

  // table start
  std::cout << std::left << std::setw(30) << "Readme details" << std::endl;
}