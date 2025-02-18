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
  this->myResults = std::make_shared<scanResults>();
  this->repoPath = REPOSITORY_PATH;
  this->fileManagerPtr = new fileManager;
}

Scanner::~Scanner() {
  delete this->mySearcher;
  delete this->fileManagerPtr;
  delete this->mySearcher;
}

void Scanner::scanForWorkflow() {

  // Workflow scan has to first make sure the requried Dir exists

  if (fileManagerPtr->dirExists(WORKFLOW_PATH) == false) {

    myResults->foundMap[WORKFLOW_STRING] = false;

  } else {

    std::vector<std::string> workflowFileExtensions = {".yml", ".yaml"};

    scanFor(workflowFileExtensions, WORKFLOW_STRING);
  }
}

void Scanner::scanFor(std::vector<std::string> searchAlts,
                      const std::string NAME) {

  std::vector<std::string> resultVectorPaths;
  bool foundAtleastOnce = false;
  for (int i = 0; i < searchAlts.size(); i++) {

    resultVectorPaths = mySearcher->searchFor(REPOSITORY_PATH, searchAlts[i]);

    if (resultVectorPaths.size() == 0 && foundAtleastOnce == false) {
      // no paths meaning found nothing
      this->setFoundMap(false, NAME);
    } else {
      // found one or more readme files
      foundAtleastOnce = true;
      this->setFoundMap(true, NAME);

      for (int i = 0; i < resultVectorPaths.size(); i++) {
        // Push back each path to the result struct
        myResults->pathsMap[NAME].push_back(resultVectorPaths[i]);
      }
    }
  }
}

void Scanner::setFoundMap(bool isFound, std::string Name) {
  if (isFound == true) {
    myResults->foundMap[Name] = true;
  }
}

void Scanner::pushBackPath(std::pair<std::string, std::string> nameAndPath) {

  myResults->pathsMap[nameAndPath.first].push_back(nameAndPath.second);
}

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

Searcher::Searcher(Scanner *scanner) { this->myScanner = scanner; }

std::vector<std::string> Searcher::searchFor(std::string path,
                                             std::string searchFor) {
  std::vector<std::string> pathsToFoundFiles;
  std::string pattern("(" + searchFor + "$)");

  std::regex r(pattern);

  for (auto const &dir_entry :
       std::filesystem::recursive_directory_iterator(path)) {
    std::string filenamePath = dir_entry.path().string(); // Convert to string

    if (regex_search(filenamePath, r)) {

      pathsToFoundFiles.push_back(filenamePath);
    }
  }

  return pathsToFoundFiles;
};
