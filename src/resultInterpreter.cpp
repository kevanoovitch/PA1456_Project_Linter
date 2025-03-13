#include "resultInterpreter.h"
#include "configHandler.h"
#include "constants.h"
#include "errorStatus.h"
#include "git2.h"
#include "scanner.h"
#include <bits/stdc++.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
using namespace constants;

/**********************************************************
 *                  Result Interpreter                    *
 **********************************************************/

resultInterpreter::resultInterpreter(const inputHandler &inputHandler) {
  this->sharedResult = inputHandler.sharedResult;
}

bool resultInterpreter::isFound(std::string resultEntry) {

  std::unordered_map<std::string, bool> &map = sharedResult->foundMap;
  auto it = map.find(resultEntry);
  if (it != map.end()) {
    if (it->second == true) {
      return true;
    }
  }
  return false;
}

void resultInterpreter::interpretLeaks() {

  std::unique_ptr<resultEntry> entryPtr =
      std::make_unique<leaksEntry>(sharedResult);
  entryPtr->entryName = LEAK_STRING;
  this->AllResultEntries.push_back(std::move(entryPtr));
}

void resultInterpreter::interpretResults() {

  // Migrate data
  // Look through the whole found map
  for (auto it : sharedResult->foundMap) {

    std::string currrentName = it.first;

    // make an entry
    std::unique_ptr<resultEntry> entryPtr =
        std::move(this->pickAndCreateEntry(currrentName));

    // Copy result attributes
    entryPtr->entryName = currrentName;
    entryPtr->isFound = it.second;

    // move the paths from the map to the entry object
    auto vectorIt = sharedResult->pathsMap.find(currrentName);
    std::vector<std::string> oldPathsCollection;
    if (vectorIt != sharedResult->pathsMap.end()) {
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
    return std::make_unique<readmeEntry>(sharedResult);
  } else if (name == LICENSE) {
    return std::make_unique<licenseEntry>(sharedResult);
  } else if (name == WORKFLOW_STRING) {
    return std::make_unique<workflowEntry>(sharedResult);
  } else if (name == GIT_IGNORE) {
    return std::make_unique<gitignoreEntry>(sharedResult);
  } else if (name == TEST_STRING) {
    return std::make_unique<testEntry>(sharedResult);
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

bool resultInterpreter::compareContributors(
    const std::pair<std::string, int> &a,
    const std::pair<std::string, int> &b) {
  return a.second > b.second; // Sort in descending order (higher count first)
}

std::vector<std::pair<std::string, int>> resultInterpreter::rankContributors() {
  // Sort the map
  std::vector<std::pair<std::string, int>> vec;
  auto &map = this->sharedResult->contributorCounts;

  for (auto &it : map) {
    vec.push_back(it);
  }

  std::sort(vec.begin(), vec.end(), this->compareContributors);

  return vec;
}

void resultInterpreter::printGitAttributes() {
  auto contributorsVec = this->rankContributors();

  fmt::print("\n📌 Repository Stats\n");
  fmt::print("═════════════════════════════\n");
  fmt::print("🔢 Total Commits: {}\n", this->sharedResult->resultNrOfCommits);
  fmt::print("👥 Contributors: (Ranked highest to lowest)\n");

  // Print each contributor in a clean format.
  for (const auto &contributor : contributorsVec) {
    fmt::print("{}: {} commits\n", contributor.first, contributor.second);
  }

  fmt::print("═════════════════════════════\n");
}

void resultInterpreter::printMinimal() {
  // Print minimal output
  std::cout << "Exit code: " << errorStatus::statusInt << std::endl;
  std::cout << "Message: " << errorStatus::statusMessage << std::endl;
}

/**********************************************************
 *                  Result Entry Strategy                 *
 **********************************************************/
resultEntry::resultEntry(std::shared_ptr<scanResults> res) {
  this->sharedResult = res;
  this->Indication = WHITE;
}

resultEntry::resultEntry() { this->Indication = WHITE; }

void resultEntry::VerifyIfFound(std::string name) {

  // paths: are paths to all files/dirs that matches the entry
  // crossRefrencing will remove all paths that are ignored by the gitIgnore

  this->handleCrossRefrence(this->paths);

  if (this->paths.empty()) {
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

void resultEntry::applyFileConfigRules() {

  auto &fileReqs = config::fileReqs[this->entryName].properties;

  this->allowMultiple = fileReqs["allowMultiple"];
  this->requireContent = fileReqs["requiresContent"];
  this->required = fileReqs["required"];
}

void resultEntry::parentPrintEntry() {

  std::vector<std::string> paths = this->paths;
  fmt::print("{} {}\n", this->Indication, this->entryName);
  fmt::print("    Reason: {}\n", this->IndicationReason);

  if (this->Indication == RED) {
    fmt::print("    Read more:\n    {}\n", this->readMore);
  }

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

std::string resultEntry::makeRelToGitRoot(std::string absPath) {

  std::filesystem::path repoRoot =
      std::filesystem::canonical(this->sharedResult->pathToRepo);
  std::filesystem::path absWherePath = std::filesystem::canonical(absPath);
  std::filesystem::path relWherePath =
      std::filesystem::relative(absWherePath, repoRoot);

  return relWherePath;
}

void resultEntry::handleCrossRefrence(std::vector<std::string> &vec) {

  for (auto it = vec.begin(); it != vec.end();) {
    // Check contents

    std::string pathRelativeToGit = this->makeRelToGitRoot(*it);

    if (crossRefrenceIgnore(pathRelativeToGit) == true) {

      // remove paths that are ignored

      it = vec.erase(it);

    } else {
      it++;
    }
  }
}

bool resultEntry::crossRefrenceIgnore(std::string path) {

  auto &repo = this->sharedResult->repo;

  int crossRefResult = 0;

  git_ignore_path_is_ignored(&crossRefResult, repo, path.c_str());

  // If crossRefResult is 1, the path is ignored;
  if (crossRefResult == 1) {
    return true;
  }

  return false;
}

void resultEntry::implicitIndication() {

  if (Indication == GREEN) {
    this->IndicationReason = NIL;
    this->Indication = GREEN;
    return;
  }

  if (Indication == WHITE) {

    this->IndicationReason = NO_CHECKS;
  }
}

void resultEntry::updateError() {
  if (this->Indication == RED || this->Indication == YELLOW) {
    errorStatus::statusInt = 1;
    errorStatus::statusMessage = "Failed linting: " + this->entryName +
                                 "Reason: " + this->IndicationReason;
  }
}

/**********************************************************
 *                        ReadMe                           *
 **********************************************************/
readmeEntry::readmeEntry(std::shared_ptr<scanResults> res) {
  this->sharedResult = res;
  this->Indication = WHITE;
}

void readmeEntry::indicatorDeterminator() {

  // Prepare the link to read more
  readMore = LINK_README;

  applyFileConfigRules();

  // check if not found --> red

  if (this->required == true) {
    this->VerifyIfFound(README);

    if (this->allowMultiple == false) {
      // check if several --> yellow
      this->noMoreThanOne(README);
    }

    if (this->requireContent == true) {
      if (this->Indication != YELLOW) {
        /* If not several were found */
        // check contents --> yellow/red
        this->checkContents();
      }
    }
  }

  // Implicit indication if white no checks if green all passed
  implicitIndication();

  updateError();
}

void readmeEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        License                         *
 **********************************************************/
licenseEntry::licenseEntry(std::shared_ptr<scanResults> res) {
  this->Indication = WHITE;
  this->sharedResult = res;
}

void licenseEntry::indicatorDeterminator() {

  // Prepare the link to read more
  readMore = LINK_LICENSE;

  applyFileConfigRules();

  if (this->required == true) {
    // check if not found --> red
    this->VerifyIfFound(LICENSE);

    if (this->allowMultiple == false) {
      // check if several --> yellow
      this->noMoreThanOne(LICENSE);
    }

    if (this->requireContent == true) {
      if (this->Indication != YELLOW) {
        /* If not several were found */
        // check contents --> yellow/red
        this->checkContents();
      }
    }
  }

  // Implicit indication
  implicitIndication();

  updateError();
}

void licenseEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        Workflow                        *
 **********************************************************/

void workflowEntry::indicatorDeterminator() {

  // Prepare the link to read more
  readMore = LINK_WORKFLOW;

  applyFileConfigRules();

  if (this->required == true) {
    // check if not found --> red
    this->VerifyIfFound(WORKFLOW_STRING);

    if (this->allowMultiple == false) {
      // check if several --> yellow
      this->noMoreThanOne(WORKFLOW_STRING);
    }

    if (this->requireContent == true) {
      this->checkContents();
    }
  }

  // Implicit indication
  implicitIndication();

  updateError();
}

void workflowEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        gitIgnore                       *
 **********************************************************/

void gitignoreEntry::indicatorDeterminator() {

  // Prepare the link to read more
  readMore = LINK_IGNORE;

  applyFileConfigRules();

  if (this->required == true) {

    // check if not found --> red
    this->VerifyIfFound(WORKFLOW_STRING);

    if (this->allowMultiple == false) {
      // check if several --> yellow
      this->noMoreThanOne(WORKFLOW_STRING);
    }

    if (this->requireContent == true) {
      this->checkContents();
    }
  }

  // Implicit indication
  implicitIndication();

  updateError();
}

void gitignoreEntry::printEntry() { parentPrintEntry(); }

/**********************************************************
 *                        Leaks Entry                     *
 **********************************************************/

void leaksEntry::indicatorDeterminator() {

  // check if found --> red
  auto &leaksMap = sharedResult->leaksReasonAndFilepathSet;

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
    this->IndicationReason = NIL;
  }

  updateError();
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

  // Prepare the link to read more
  readMore = LINK_TESTS;

  applyFileConfigRules();

  int countValidTestFiles = 0;

  if (this->required == false) {
    // if not required --> white
    implicitIndication();
    return;
  }

  // create a fileManager object
  fileManager testFsHelper;

  // cross refrence and remove all files that are ignore accroding to the
  // gitIgnore

  auto &vec = this->paths;

  handleCrossRefrence(vec);

  if (vec.empty()) {
    /* No unit test found which is bad */
    this->Indication = RED;
    this->IndicationReason = "No unit tests found!";
    return;
  }

  // for each path entry

  if (this->requireContent == true) {
    for (auto it = vec.begin(); it != vec.end();) {
      // Check contents

      if (!testFsHelper.checkContentsIsEmpty(*it)) {

        // remove paths that lead to testing dir/files with contents

        it = vec.erase(it);
        countValidTestFiles++;

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
    this->IndicationReason = "These testing entries lacked content";
    updateError();
  }

  if (allowMultiple == false && countValidTestFiles > 1) {
    // check if several --> yellow
    this->Indication = YELLOW;
    this->IndicationReason = "Several test files were found";
    updateError();
    return;
  }
}

void testEntry::printEntry() { parentPrintEntry(); }
