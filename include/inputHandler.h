#pragma once

#include "fileManager.h"
#include "resultInterpreter.h"
#include <git2.h>
#include <gtest/gtest.h>
#include <string>

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Strategy; // Forward declare Strategy since we just need a pointer in
                // inputHandler

/**********************************************************
 *                          Scan Results                  *
 **********************************************************/

struct scanResults {
  git_repository *repo;
  std::string pathToRepo;
  std::unordered_map<std::string, bool> foundMap;
  std::unordered_map<std::string, std::vector<std::string>> pathsMap;

  int resultNrOfCommits;
  std::unordered_map<std::string, int>
      contributorCounts; // name : amount of commits

  std::unordered_map<std::string, std::set<std::string>>
      leaksReasonAndFilepathSet;
  // Default constructor for struct
  scanResults();
  ~scanResults();
};

/**********************************************************
 *                  InputHandler                          *
 **********************************************************/

class inputHandler {

public:
  inputHandler();
  ~inputHandler();
  bool getIsUrl();
  void setInput(std::string const in);

  void setProcessSuccess(bool flag);
  void pickStrategy(std::string input);
  void executeStrategy();
  std::string getInput();
  Strategy *getStrategyPtr();
  std::string localPath;
  bool getProcessSuccess();

  std::shared_ptr<scanResults> sharedResult = std::make_shared<scanResults>();

private:
  bool argumentChecker(std::string arg);
  bool processSuccess;
  bool isUrl;
  fileManager *fileManagerPtr;
  std::string input;
  Strategy *inputStrategy; // a pointer for what type of child it is

  /*Test related*/
  FRIEND_TEST(inputHandler,
              inputArgsTest); // Allow specific test to access private members
  FRIEND_TEST(typeURL,
              URLhandling); // Allow specific test to access private members'
  FRIEND_TEST(typeFolder,
              folderHandling); // Allow specific test to access private members
};

/**********************************************************
 *                  Strategy                              *
 **********************************************************/

class Strategy : public inputHandler {

public:
  virtual std::string getInput() = 0;
  virtual ~Strategy();
  virtual void proccessInput() = 0;

protected:
  bool handleLibGit2Error(int err);
};

/**********************************************************
 *                  typeURL                               *
 **********************************************************/

class typeURL : public Strategy {
public:
  typeURL(inputHandler *h);
  ~typeURL();

  std::string getInput() override;
  void proccessInput() override;

private:
  inputHandler *parentInputHandler;
};

/**********************************************************
 *                  typeFolder                            *
 **********************************************************/
class typeFolder : public Strategy {
public:
  typeFolder(inputHandler *h);
  ~typeFolder();
  bool checkOwnershipIssue(const std::string &path);
  std::string getInput() override;
  void proccessInput() override;

private:
  inputHandler *parentInputHandler;
};
