#include "scanner.h"
#include "constants.h"
#include "regex"
#include <functional>
#include <unordered_map>
#include <utility>

using namespace constants;

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

Scanner::Scanner() {
  this->mySearcher = new Searcher(this);
  this->myResults = new scanResults;
  this->repoPath = REPOSITORY_PATH;
  this->fileManagerPtr = new fileManager;
}

Scanner::~Scanner() {
  delete this->mySearcher;
  delete this->myResults;
  delete this->fileManagerPtr;
}

void Scanner::scanForGitignore() {
  std::pair<bool, std::string> searchResult =
      this->mySearcher->searchFor(REPOSITORY_PATH, GIT_IGNORE);

  if (searchResult.first == true) {
    this->setGitIgnore(searchResult.first, searchResult.second);
    // processSearchResults(searchResult.second, GIT_IGNORE);

  } else {
    this->setGitIgnore(searchResult.first, searchResult.second);
  }
}

bool Scanner::scanForWorkflow() {

  if (fileManagerPtr->dirExists(WORKFLOW_PATH) == false) {

    myResults->foundMap[WORKFLOW_STRING] = false;

    // search the whole repo for yamls files and or /workflow

    return true;
  }

  std::pair<bool, std::string> ymlResult =
      mySearcher->searchFor(WORKFLOW_PATH, ".yml");

  std::pair<bool, std::string> yamlResult =
      mySearcher->searchFor(WORKFLOW_PATH, ".yaml");

  if (ymlResult.first || yamlResult.first == true) {
    if (ymlResult.first == true) {
      this->setworkflow(ymlResult.first, ymlResult.second);
    } else {
      this->setworkflow(yamlResult.first, yamlResult.second);
    }
  }

  return false;
}

void Scanner::setGitIgnore(bool found, std::string path) {

  if (found == true) {
    myResults->foundMap[GIT_IGNORE] = true;
    myResults->gitIgnoreHandle = path;

  } else {
    myResults->foundMap[GIT_IGNORE] = false;
  }
}
void Scanner::setlicense(bool found, std::string path) {

  if (found == true) {
    myResults->foundMap["license"] = true;
    myResults->gitIgnoreHandle = path;
  } else {
    myResults->foundMap["license"] = false;
  }
}
void Scanner::setworkflow(bool found, std::string path) {

  if (found == true) {
    myResults->foundMap[WORKFLOW_STRING] = true;
    myResults->gitIgnoreHandle = path;
  } else {
    myResults->foundMap[WORKFLOW_STRING] = false;
  }
}

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

Searcher::Searcher(Scanner *scanner) { this->myScanner = scanner; }

std::pair<bool, std::string> Searcher::searchFor(std::string path,
                                                 std::string searchFor) {

  std::string pattern("(" + searchFor + "$)");

  std::regex r(pattern);

  for (auto const &dir_entry :
       std::filesystem::recursive_directory_iterator(path)) {
    std::string filenamePath = dir_entry.path().string(); // Convert to string

    if (regex_search(filenamePath, r)) {

      // myScanner->processSearchResults(filenamePath, searchFor);
      //  std::cout << filenamePath << std::endl;

      return {true, filenamePath};
    }
  }

  return {false, ""};
};
