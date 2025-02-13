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
  std::cout << std::left << std::setw(20) << "Requried File" << " | Result\n";
  std::cout << std::string(30, '-') << "\n"; // Separator line

  std::string resultOutput;
  std::string RequriedFile;

  for (const auto &pair : map) {

    if (pair.second == false) {
      resultOutput = "RED";
      RequriedFile = pair.first + " not found ";
    }
    if (pair.second == true) {
      resultOutput = "GREEN";
      RequriedFile = pair.first + " found ";
    }

    std::cout << std::left << std::setw(20) << RequriedFile << " | "
              << std::boolalpha << resultOutput << "\n";
  }
}
