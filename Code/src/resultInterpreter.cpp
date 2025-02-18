#include "resultInterpreter.h"
#include "constants.h"

using namespace constants;

/**********************************************************
 *                  Result Interpreter                    *
 **********************************************************/

resultInterpreter::resultInterpreter(std::shared_ptr<scanResults> res) {
  this->results = res;
}

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

void resultInterpreter::interpretResults() {

  // Migrate data
  // Look through the whole found map
  for (auto it : results->foundMap) {

    std::string currrentName = it.first;

    // make an entry
    std::unique_ptr<resultEntry> entryPtr =
        std::move(this->pickAndCreateEntry(currrentName));

    // Copy result attributes
    entryPtr->entryName = currrentName;
    entryPtr->isFound = it.second;

    // move the paths from the map to the entry object
    auto vectorIt = results->pathsMap.find(currrentName);
    std::vector<std::string> oldPathsCollection;
    if (vectorIt != results->pathsMap.end()) {
      oldPathsCollection = vectorIt->second; // Reference to existing vector

    } else {
      std::cerr << "Key not found\n";
    }

    entryPtr->paths = oldPathsCollection; // migrate the paths

    this->AllResultEntries.push_back(std::move(entryPtr));
  }

  // Call each indicateDeterminator afterwards

  for (auto &it : this->AllResultEntries) {
    it->indicatorDeterminator();
  }
}

std::unique_ptr<resultEntry>
resultInterpreter::pickAndCreateEntry(std::string name) {
  if (name == README) {
    return std::make_unique<readmeEntry>(results);
  } else if (name == LICENSE) {
    return std::make_unique<licenseEntry>(results);
  } else if (name == WORKFLOW_STRING) {
    return std::make_unique<workflowEntry>(results);
  } else if (name == GIT_IGNORE) {
    return std::make_unique<gitignoreEntry>(results);
  } else {
    throw std::runtime_error("Unknown entry type: " + name);
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
  for (auto &it : this->AllResultEntries) {
    it->printEntry();
  }
}

/**********************************************************
 *                          Scan Results                  *
 **********************************************************/

scanResults::scanResults() {

  this->foundMap[GIT_IGNORE] = false;
  this->foundMap[WORKFLOW_STRING] = false;
  this->foundMap[LICENSE] = false;
  this->foundMap[README] = false;
}

/**********************************************************
 *                  Result Entry Strategy                 *
 **********************************************************/
resultEntry::resultEntry(std::shared_ptr<scanResults> res) {
  this->myResults = res;
}

void resultEntry::VerifyIfFound(std::string name) {
  if (myResults->foundMap[name] == false) {
    this->Indication = RED;
    this->Indication = "Reason: Was not found";
  } else {
    this->Indication = true;
  }
}

void resultEntry::noMoreThanOne(std::string name) {
  if (this->paths.size() > 1) {
    this->Indication = YELLOW;
    this->IndicationReason = "There we several" + name + " files";
  }
}

void resultEntry::parentPrintEntry() {
  std::cout << this->Indication << " | " << this->entryName << " | "
            << this->IndicationReason << std::endl;
}

/**********************************************************
 *                        ReadMe                           *
 **********************************************************/

void readmeEntry::indicatorDeterminator() {

  // check if not found --> red
  this->VerifyIfFound(README);

  // check if several --> yellow
  this->noMoreThanOne(README);
  // check contents --> yellow/red
}

void readmeEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        License                         *
 **********************************************************/

void licenseEntry::indicatorDeterminator() {

  // check if not found --> red
  this->VerifyIfFound(LICENSE);

  // check if several --> yellow
  this->noMoreThanOne(LICENSE);

  // check contents --> yellow/red
}

void licenseEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        Workflow                        *
 **********************************************************/

void workflowEntry::indicatorDeterminator() {

  // check if not found --> red
  this->VerifyIfFound(WORKFLOW_STRING);

  // check if several --> yellow
  this->noMoreThanOne(WORKFLOW_STRING);

  // check contents --> yellow/red
}

void workflowEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        gitIgnore                       *
 **********************************************************/

void gitignoreEntry::indicatorDeterminator() {

  // check if not found --> red
  this->VerifyIfFound(GIT_IGNORE);

  // check if several --> yellow
  this->noMoreThanOne(GIT_IGNORE);

  // check contents --> yellow/red
}

void gitignoreEntry::printEntry() { parentPrintEntry(); }