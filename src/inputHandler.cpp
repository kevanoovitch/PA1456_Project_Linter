#include "inputHandler.h"
#include "constants.h"
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

  if (int err = git_clone(&parentInputHandler->sharedResult->repo, url.c_str(),
                          localPath.c_str(), NULL) != 0) {
    // error in cloning

    parentInputHandler->setProcessSuccess(false);
  } else {
    parentInputHandler->setProcessSuccess(true);
  }
}

/**********************************************************
 *                Folder Implementation                   *
 **********************************************************/

typeFolder::~typeFolder() {}

typeFolder::typeFolder(inputHandler *h) { this->parentInputHandler = h; }

std::string typeFolder::getInput() { return parentInputHandler->getInput(); }

void typeFolder::proccessInput() {

  std::string path = getInput();

  // ensure target folder exists
  fileManager filesys;
  filesys.ensureFolderExists(REPOSITORY_PATH);

  // set the repo
  int error = git_repository_init(&parentInputHandler->sharedResult->repo,
                                  path.c_str(), false);

  // Need to change the scan dir

  parentInputHandler->localPath = path;
}