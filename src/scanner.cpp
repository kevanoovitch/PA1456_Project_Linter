#include "scanner.h"
#include "constants.h"
#include "regex"
#include <array>
#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
using json = nlohmann::json;

using namespace constants;

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

Scanner::Scanner(const inputHandler &inputHandler) {
  this->mySearcher = new Searcher(this);
  this->sharedResult = inputHandler.sharedResult;
  this->repoPath = inputHandler.localPath;
  this->fileManagerPtr = new fileManager;
  this->myGitScanner = new GitScanner(this);
}

Scanner::~Scanner() {
  delete this->mySearcher;
  delete this->fileManagerPtr;
  delete this->myGitScanner;
}

void Scanner::scanForWorkflow() {

  // Workflow scan has to first make sure the requried Dir exists

  if (fileManagerPtr->dirExists(this->repoPath + "/.github/workflows") ==
      false) {

    sharedResult->foundMap[WORKFLOW_STRING] = false;

  } else {

    std::vector<std::string> workflowFileExtensions = {".yml", ".yaml"};

    scanFor(workflowFileExtensions, WORKFLOW_STRING);
  }
}

void Scanner::scanForTest(std::vector<std::string> searchAlts,
                          const std::string NAME) {
  std::vector<std::string> resultVectorPaths;
  bool foundAtleastOnce = false;
  for (int i = 0; i < searchAlts.size(); i++) {

    resultVectorPaths = mySearcher->contains(this->repoPath, searchAlts[i]);

    if (resultVectorPaths.size() == 0 && foundAtleastOnce == false) {
      // no paths meaning found nothing
      this->setFoundMap(false, NAME);
    } else {
      // found one or more readme files
      foundAtleastOnce = true;
      this->setFoundMap(true, NAME);

      for (int i = 0; i < resultVectorPaths.size(); i++) {
        // Push back each path to the result struct
        sharedResult->pathsMap[NAME].push_back(resultVectorPaths[i]);
      }
    }
  }
}

void Scanner::scanFor(std::vector<std::string> searchAlts,
                      const std::string NAME) {

  std::vector<std::string> resultVectorPaths;
  bool foundAtleastOnce = false;
  for (int i = 0; i < searchAlts.size(); i++) {

    resultVectorPaths = mySearcher->endsWithFile(this->repoPath, searchAlts[i]);

    if (resultVectorPaths.size() == 0 && foundAtleastOnce == false) {
      // no paths meaning found nothing
      this->setFoundMap(false, NAME);
    } else {
      // found one or more readme files
      foundAtleastOnce = true;
      this->setFoundMap(true, NAME);

      for (int i = 0; i < resultVectorPaths.size(); i++) {
        // Push back each path to the result struct
        sharedResult->pathsMap[NAME].push_back(resultVectorPaths[i]);
      }
    }
  }
}

void Scanner::setFoundMap(bool isFound, std::string Name) {
  if (isFound == true) {
    sharedResult->foundMap[Name] = true;
  }
}

void Scanner::pushBackPath(std::pair<std::string, std::string> nameAndPath) {

  sharedResult->pathsMap[nameAndPath.first].push_back(nameAndPath.second);
}

void Scanner::scanGitAttributes() {

  if (this->sharedResult->repo == nullptr) {
    std::cerr << "Error in Scanner::scanGitAttributes() repo was nullptr\n";
    return;
  }

  // count commits
  int nrOfCommits = this->myGitScanner->countCommits(this->sharedResult->repo);

  // build set of contributors
  std::set<std::string> Contributors =
      this->myGitScanner->countContributors(this->sharedResult->repo);

  // Write results to struct

  this->sharedResult->resultContributors = Contributors;
  this->sharedResult->resultNrOfCommits = nrOfCommits;
}

void Scanner::setRepoPath(std::string path) { this->repoPath = path; }

void Scanner::parseGitleaksOutput(const std::string &jsonFilePath) {
  std::ifstream file(jsonFilePath);
  if (!file) {
    std::cerr << "Failed to open Gitleaks JSON output." << std::endl;
    return;
  }

  nlohmann::json report;
  file >> report;

  if (!report.empty()) {

    // 🔹 Loop through each detected secret
    for (const auto &leak : report) {

      auto &leaksmap = this->sharedResult->leaksReasonAndFilepathSet;

      leaksmap[leak["Description"]].insert(leak["File"]);
    }
  } else {
    // Do nothing resultInterpreter will handle the conclusion
  }
}

void Scanner::runGitLeaks() {

  std::string command = "gitleaks dir " + repoPath + " -r " +
                        PATH_REPORT_CREDENTIALS + " --no-banner";

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    std::cerr << "Failed to run Gitleaks" << std::endl;
    return;
  }

  // Read output
  char buffer[128];
  std::string result;
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }

  // Close the pipe
  pclose(pipe);

  parseGitleaksOutput(PATH_REPORT_CREDENTIALS);
}

std::string Scanner::getRepoPath() { return this->repoPath; }

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

Searcher::Searcher(Scanner *scanner) { this->myScanner = scanner; }

std::vector<std::string> Searcher::searchFor(std::string path,
                                             std::string searchFor) {
  std::vector<std::string> pathsToFoundFiles;

  std::string pattern(searchFor + "$");

  std::regex r(pattern);

  for (auto const &dir_entry :
       std::filesystem::recursive_directory_iterator(path)) {
    std::string filenamePathRelative =
        dir_entry.path().string(); // Convert to string

    if (regex_search(filenamePathRelative, r)) {

      std::string filenamePathAbsolute =
          std::filesystem::absolute(dir_entry).string();

      pathsToFoundFiles.push_back(filenamePathAbsolute);
    }
  }

  return pathsToFoundFiles;
};

std::string Searcher::makeRelToGitRoot(std::string absPath) {

  std::filesystem::path repoRoot =
      std::filesystem::canonical(this->myScanner->getRepoPath());
  std::filesystem::path absWherePath = std::filesystem::canonical(absPath);
  std::filesystem::path relWherePath =
      std::filesystem::relative(absWherePath, repoRoot);

  return relWherePath;
}

std::vector<std::string> Searcher::search(std::string wherePath,
                                          std::string pattern) {

  std::vector<std::string> pathsToFoundFiles;

  std::regex r(pattern);

  for (auto const &dir_entry :
       std::filesystem::recursive_directory_iterator(wherePath)) {
    std::string absfilenamePath =
        dir_entry.path().string(); // Convert to string

    // make path relative
    std::string tmpRelPath = this->makeRelToGitRoot(absfilenamePath);
    // convert to lower
    std::string tmpLowerPath = this->lower(tmpRelPath);

    if (regex_search(tmpLowerPath, r)) {

      std::string filenamePathAbsolute =
          std::filesystem::absolute(dir_entry).string();

      pathsToFoundFiles.push_back(filenamePathAbsolute);
    }
  }

  return pathsToFoundFiles;
}

std::vector<std::string> Searcher::endsWithFile(std::string wherePath,
                                                std::string searchFor) {
  std::string pattern = (searchFor + "$");

  return this->search(wherePath, pattern);
}

std::vector<std::string> Searcher::contains(std::string wherePath,
                                            std::string searchFor) {

  std::string pattern = (searchFor);

  return this->search(wherePath, pattern);
}

std::string Searcher::lower(std::string str) {
  // Source:
  // https://www.geeksforgeeks.org/how-to-convert-std-string-to-lower-case-in-cpp/

  transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

/**********************************************************
 *                          gitScanner                    *
 **********************************************************/

GitScanner::GitScanner(Scanner *ptr) { this->myScanner = ptr; }

int GitScanner::countCommits(git_repository *repository) {

  int commitCount = 0;
  git_revwalk *walker;

  if (repository == nullptr) {
    std::cerr << "Error in countCommits() repoPtr was nullptr \n";
    return -1;
  }

  int error = git_revwalk_new(&walker, repository);
  error = git_revwalk_push_head(walker);

  git_oid oid;
  while (!git_revwalk_next(&oid, walker)) {
    commitCount++;
  }

  if (error != 0) {

    return -1;
  }

  git_revwalk_free(walker);
  return commitCount;
}

std::set<std::string>
GitScanner::countContributors(git_repository *repository) {
  std::set<std::string> result;

  int commitCount = 0;
  git_revwalk *walker;

  if (repository == nullptr) {

    throw std::runtime_error(
        "Error in countContributors() repoPtr was nullptr \n");
  }

  int error = git_revwalk_new(&walker, repository);
  error = git_revwalk_push_head(walker);

  git_oid oid;
  while (!git_revwalk_next(&oid, walker)) {

    git_commit *commit = nullptr;

    git_commit_lookup(&commit, repository, &oid);

    const git_signature *author = git_commit_author(commit);
    if (author) {
      std::string contributor =
          std::string(author->name) + " <" + author->email + ">";
      result.insert(contributor);
    }
  }

  git_revwalk_free(walker);

  return result;
}
