#include "resultInterpreter.h"
#include "constants.h"
#include <fmt/core.h>
#include <fmt/ranges.h>

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
  std::cout << "Results\n";
  std::cout << std::string(45, '-');
  std::cout << "\n";

  // print all entries
  for (auto &it : this->AllResultEntries) {

    it->printEntry();
  }

  // Print GIT attributes
  printGitAttributes();
}

void resultInterpreter::printGitAttributes() {

  std::set<std::string> &set = this->results->resultContributors;

  fmt::print("\n📌 Repository Stats\n");
  fmt::print("═════════════════════════════\n");
  fmt::print("🔢 Total Commits: {}\n", this->results->resultNrOfCommits);
  fmt::print("👥 Contributors:\n{} \n", fmt::join(set, "\n"));
  fmt::print("═════════════════════════════\n");
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
  this->Indication = WHITE;
}

resultEntry::resultEntry() { this->Indication = WHITE; }

void resultEntry::VerifyIfFound(std::string name) {
  if (myResults->foundMap[name] == false) {
    this->Indication = RED;
    this->IndicationReason = "Was not found";
  } else {
    this->Indication = GREEN;
  }
}

void resultEntry::noMoreThanOne(std::string name) {
  if (this->paths.size() > 1) {
    this->Indication = YELLOW;
    this->IndicationReason = "There were several " + name + " files";
  }
}

void resultEntry::parentPrintEntry() {
  // The data

  /* Default option */
  // this->Indication = "White";

  std::vector<std::string> paths = this->paths;
  fmt::print("{} {}\n", this->Indication, this->entryName);
  fmt::print("    Reason: {}\n", this->IndicationReason);
  fmt::print("    Paths:\n");
  for (const auto &path : paths) {
    fmt::print("      - {}\n", path);
  }
  fmt::print("\n");
}

/**********************************************************
 *                        ReadMe                           *
 **********************************************************/
readmeEntry::readmeEntry(std::shared_ptr<scanResults> res) {
  this->myResults = res;
  this->Indication = WHITE;
}

void readmeEntry::indicatorDeterminator() {
  // check if not found --> red
  this->VerifyIfFound(README);

  // check if several --> yellow
  this->noMoreThanOne(README);

  // check contents --> yellow/red

  // Implicit indication
  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
}

void readmeEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        License                         *
 **********************************************************/
licenseEntry::licenseEntry(std::shared_ptr<scanResults> res) {
  this->Indication = WHITE;
  this->myResults = res;
}

void licenseEntry::indicatorDeterminator() {
  // check if not found --> red
  this->VerifyIfFound(LICENSE);

  // check contents --> yellow/red

  // Implicit indication
  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
}

void licenseEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        Workflow                        *
 **********************************************************/

void workflowEntry::indicatorDeterminator() {
  // check if not found --> red
  this->VerifyIfFound(WORKFLOW_STRING);

  // Implicit indication
  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
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

  // Implicit indication
  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
}

void gitignoreEntry::printEntry() { parentPrintEntry(); }