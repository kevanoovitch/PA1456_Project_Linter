#pragma once

#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include <filesystem>
#include <git2.h>
#include <unordered_map>
#include <utility>

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Searcher;
struct scanResults;
class GitScanner;
class inputHandler;

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

class Scanner {
public:
  Scanner(const inputHandler &inputHandler);
  ~Scanner();

  void scanForWorkflow();
  void scanFor(std::vector<std::string> searchAlts, std::string name);
  void pushBackPath(std::pair<std::string, std::string> entry);
  void scanGitAttributes();
  void scanForTest(std::vector<std::string> searchAlts, std::string name);

  void runGitLeaks();
  void parseGitleaksOutput(const std::string &jsonFilePath);

  void setRepoPath(std::string path);
  void setFoundMap(bool isFound, const std::string name);
  std::string getRepoPath();

  std::shared_ptr<scanResults> sharedResult;

private:
  Searcher *mySearcher;
  std::string repoPath; // at construction is root
  fileManager *fileManagerPtr;
  GitScanner *myGitScanner;

  /*Test related*/
  FRIEND_TEST(
      ScannerOperations,
      findRequiredFiles); // Allow specific test to access private members
  FRIEND_TEST(
      ScannerOperations,
      dontfindRequiredFiles); // Allow specific test to access private members
  FRIEND_TEST(
      ScannerGitOperations,
      findGitAtributes); // Allow specific test to access private members
};

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

class Searcher {
public:
  Searcher(Scanner *scanner);
  std::vector<std::string> searchFor(std::string wherePath,
                                     std::string searchFor);
  std::vector<std::string> endsWithFile(std::string wherePath,
                                        std::string searchFor);
  std::vector<std::string> contains(std::string wherePath,
                                    std::string searchFor);

private:
  Scanner *myScanner;
  std::vector<std::string> search(std::string wherePath, std::string pattern);
  std::string lower(std::string str);
  std::string makeRelToGitRoot(std::string absPath);
};

/**********************************************************
 *                          GitScanner                    *
 **********************************************************/

class GitScanner {
public:
  GitScanner(Scanner *scanner);
  std::unordered_map<std::string, int>
  countContributors(git_repository *repository);
  int countCommits(git_repository *repository);

private:
  Scanner *myScanner;
};