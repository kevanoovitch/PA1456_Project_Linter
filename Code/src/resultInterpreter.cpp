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

void resultInterpreter::interpretLeaks() {

  std::unique_ptr<resultEntry> entryPtr = std::make_unique<leaksEntry>(results);
  entryPtr->entryName = "Credential Leaks";
  this->AllResultEntries.push_back(std::move(entryPtr));
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
    }

    entryPtr->paths = oldPathsCollection; // migrate the paths

    this->AllResultEntries.push_back(std::move(entryPtr));
  }

  // Interpret leaks handler since they will never appear in the found map
  interpretLeaks();

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
  } else if (name == TEST_STRING) {
    return std::make_unique<testEntry>(results);
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
  this->foundMap[TEST_STRING] = false;
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

  std::vector<std::string> paths = this->paths;
  fmt::print("{} {}\n", this->Indication, this->entryName);
  fmt::print("    Reason: {}\n", this->IndicationReason);
  fmt::print("    Paths:\n");
  for (const auto &path : paths) {
    fmt::print("      - {}\n", path);
  }
  fmt::print("\n");
}

void resultEntry::checkContents() {

  fileManager testFsHelper;

  // for each path entry
  auto &vec = this->paths;
  for (auto it = vec.begin(); it != vec.end();) {
    // Check contents

    if (!testFsHelper.checkContentsIsEmpty(*it)) {

      // remove paths that lead to testing dir/files with contents

      it = vec.erase(it);

    } else {
      it++;
    }
  }

  // Determine yellow if the vector is not empty

  if (!vec.empty()) {
    /* all paths had contents */

    this->Indication = YELLOW;
    this->IndicationReason = "Theese files/dirs had no content";
    return;
  }
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
  this->checkContents();

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
  this->checkContents();

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
  this->checkContents();

  // Implicit indication
  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
}

void gitignoreEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        Leaks Entry                     *
 **********************************************************/

void leaksEntry::indicatorDeterminator() {
  // check if found --> red
  auto &leaksMap = myResults->leaksReasonAndFilepathSet;

  if (leaksMap.empty()) {
    this->Indication = GREEN;
  } else {
    // There were leaks
    this->Indication = RED;

    for (auto &leak : leaksMap) {
      this->IndicationReasons.push_back(leak);
    }
  }

  if (Indication == GREEN) {
    this->IndicationReason = "No issues detected";
  }
}

void leaksEntry::printEntry() {

  std::vector<std::pair<std::string, std::set<std::string>>> reasons =
      this->IndicationReasons;
  fmt::print("{} {}\n", this->Indication, this->entryName);
  if (!this->IndicationReason.empty()) {
    fmt::print("    Reason: {}\n", this->IndicationReason);
  } else {
    for (const auto &it : reasons) {
      fmt::print("    Description: {}\n", it.first);

      // Print each file path on a new line
      for (const auto &file : it.second) {
        fmt::print("      - {}\n", file);
      }
    }
  }

  fmt::print("\n");
}

/**********************************************************
 *                        Tests Entry                     *
 **********************************************************/
void testEntry::indicatorDeterminator() {

  // create a fileManager object
  fileManager testFsHelper;

  if (myResults->foundMap[TEST_STRING] == false) {
    /* No unit test found which is bad */
    this->Indication = RED;
    this->IndicationReason = "No unit tests found!";
    return;
  }

  // for each path entry
  auto &vec = this->paths;
  for (auto it = vec.begin(); it != vec.end();) {
    // Check contents

    if (!testFsHelper.checkContentsIsEmpty(*it)) {

      // remove paths that lead to testing dir/files with contents

      it = vec.erase(it);

    } else {
      it++;
    }
  }

  // make determinations

  if (vec.empty()) {
    /* all paths had contents */

    this->Indication = GREEN;
    this->IndicationReason = "No issues detected";
    return;
  }

  // Some paths were empty

  this->Indication = YELLOW;
  this->IndicationReason = "Theese testing entries lacked content";
}

void testEntry::printEntry() { parentPrintEntry(); }
