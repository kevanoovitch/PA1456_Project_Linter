#pragma once

#include "fileManager.h"
#include "resultInterpreter.h"
#include <filesystem>
#include <unordered_map>
#include <utility>

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Searcher;
struct scanResults;

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

class Scanner {
public:
  Scanner();
  ~Scanner();

  void scanForWorkflow();

  void scanFor(std::vector<std::string> searchAlts, std::string name);
  void setFoundMap(bool isFound, const std::string name);

  void pushBackPath(std::pair<std::string, std::string> entry);
  std::shared_ptr<scanResults> myResults;

private:
  Searcher *mySearcher;
  std::string repoPath;
  fileManager *fileManagerPtr;

  // std::string gitIgnoreHandle;
  // std::string licenseHandle;
  // std::string workflowHandle;

  /*Test related*/
  FRIEND_TEST(
      ScannerOperations,
      findRequiredFiles); // Allow specific test to access private members
  FRIEND_TEST(
      ScannerOperations,
      dontfindRequiredFiles); // Allow specific test to access private members
};

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

class Searcher {
public:
  Searcher(Scanner *scanner);
  std::vector<std::string> searchFor(std::string wherePath,
                                     std::string searchFor);

private:
  Scanner *myScanner;
};