#include "inputHandler.h"
#include "constants.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fileManager.h"
#include <git2.h>
#include <regex>

using namespace constants;

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

scanResults::~scanResults() {}

inputHandler::inputHandler() {
  this->isUrl = false;
  this->setInput("");
  this->localPath = REPOSITORY_PATH;
  this->inputStrategy = nullptr;
  this->setProcessSuccess(false);

  this->fileManagerPtr = new fileManager;
}

inputHandler::~inputHandler() {
  delete this->inputStrategy;
  delete this->fileManagerPtr;
}

void inputHandler::pickStrategy(std::string input) {

  if (this->argumentChecker(input) == false) {

    this->inputStrategy = nullptr;
    return;
  } else {

    if (this->isUrl == true) {
      // It was a valid URL input
      this->inputStrategy = new typeURL(this);
    } else {
      this->inputStrategy = new typeFolder(this);
    }
  }

  this->setInput(input);
}

Strategy *inputHandler::getStrategyPtr() { return this->inputStrategy; }

bool inputHandler::argumentChecker(std::string arg) {
  /*Return true if valid, return false if unvalid*/

  // Regex to check valid URL

  const std::regex pattern(
      R"((https:\/\/)?github\.com\/[a-zA-Z0-9_\-]+\/[a-zA-Z0-9_\-\.]+)");

  // If the URL
  // is empty return false
  if (arg.empty()) {

    return false;
  }

  // Return true if the URL
  // matched the ReGex
  if (regex_match(arg, pattern)) {

    this->isUrl = true;
    return true;
  }

  else if (this->fileManagerPtr->checkValidRepoPath(arg)) {
    // Check if its a path to a folder
    this->isUrl = false;
    return true;
  } else {
    return false;
  }

  // IMPLICIT ELSE: It's neither folder or URL so invalid input
}

bool inputHandler::getIsUrl() { return this->isUrl; }

void inputHandler::setInput(std::string in) { this->input = in; }

void inputHandler::setProcessSuccess(bool flag) { this->processSuccess = flag; }

bool inputHandler::getProcessSuccess() { return this->processSuccess; }

void inputHandler::executeStrategy() {

  if (isUrl == true) {
    // Make sure the dir to clone to is empty.

    inputStrategy->proccessInput();
  } else {
    inputStrategy->proccessInput();
  }
}

std::string inputHandler::getInput() { return this->input; }

/**********************************************************
 *                  Strategy Implementation               *
 **********************************************************/

Strategy::~Strategy() {}

bool Strategy::handleLibGit2Error(int err) {
  if (err != 0) {
    // some failure
    const git_error *e = git_error_last();
    if (e) {
      std::cerr << "Error: Cloning failed with error code " << err << ": "
                << e->message << "\n";
    } else {
      std::cerr << "Error: Cloning failed with unknown error\n";
    }
    return false;
  } else {
    // sucess
    return true;
  }
}

/**********************************************************
 *                  URL Implementation                    *
 **********************************************************/

typeURL::~typeURL() {}

typeURL::typeURL(inputHandler *h) { this->parentInputHandler = h; }

std::string typeURL::getInput() { return parentInputHandler->getInput(); }

void typeURL::proccessInput() {
  std::string url = getInput();

  fileManager filesys;

  filesys.checkAndClear(REPOSITORY_PATH);

  int err = git_clone(&parentInputHandler->sharedResult->repo, url.c_str(),
                      localPath.c_str(), NULL);

  bool gotError = handleLibGit2Error(err);

  parentInputHandler->sharedResult->pathToRepo = localPath;

  parentInputHandler->setProcessSuccess(gotError);
}

/**********************************************************
 *                Folder Implementation                   *
 **********************************************************/

typeFolder::~typeFolder() {}

typeFolder::typeFolder(inputHandler *h) { this->parentInputHandler = h; }

std::string typeFolder::getInput() { return parentInputHandler->getInput(); }

bool typeFolder::checkOwnershipIssue(const std::string &path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0) {
    perror("stat error");
    std::cerr << "Error: Could not stat directory\n";
    parentInputHandler->setProcessSuccess(false);
    return true;
  }
  uid_t currentUid = getuid();
  if (info.st_uid != currentUid) {
    std::cerr << "Ownership issue: directory is owned by UID " << info.st_uid
              << ", but current user UID is " << currentUid << "\n";
    return true;
  }
  return false;
}

void typeFolder::proccessInput() {

  std::string path = getInput();

  // ensure target folder exists
  fileManager filesys;
  if (!filesys.dirExists(path)) {
    std::cerr << "Error: Folder does not exist\n";
    return;
  }

  // ensure we have the the requries rights/ownership

  if (checkOwnershipIssue(path)) {

    // make a copy to solve the ownership problem
    auto tmp = filesys.copyRepoToInternal(path);
    path = tmp;
  }

  // set the repo
  int error = git_repository_open(&parentInputHandler->sharedResult->repo,
                                  path.c_str());

  bool gotError = handleLibGit2Error(error);

  parentInputHandler->sharedResult->pathToRepo = localPath;

  parentInputHandler->setProcessSuccess(gotError);

  // Need to change the scan dir

  parentInputHandler->localPath = path;
  parentInputHandler->sharedResult->pathToRepo = path;
  REPOSITORY_PATH = path;
  WORKFLOW_PATH = path + "/.github/workflows";
}