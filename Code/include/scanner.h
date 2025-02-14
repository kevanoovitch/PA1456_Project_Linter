#pragma once

#include "fileManager.h"
#include <filesystem>
#include <unordered_map>
#include <utility>

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Searcher;

/**********************************************************
 *                          Scan Results                  *
 **********************************************************/

struct scanResults {
  std::unordered_map<std::string, bool> foundMap;
  // std::unordered_map<std::string, std::string> referenceMap;

  std::string gitIgnoreHandle;
  std::string licenseHandle;
  std::string workflowHandle;

  // Default constructo for struct
  scanResults();
};

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

class Scanner {
public:
  Scanner();
  ~Scanner();

  bool scanForWorkflow();
  void scanForGitignore();
  void setGitIgnore(bool found, std::string pathTo);
  void setlicense(bool found, std::string pathTo);
  void setworkflow(bool found, std::string pathTo);
  void scanForLicense();
  scanResults *myResults;

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
  std::pair<bool, std::string> searchFor(std::string path,
                                         std::string searchFor);

private:
  Scanner *myScanner;
};