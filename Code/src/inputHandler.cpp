#include "inputHandler.h"

#include <git2.h>
#include <regex>
#include <string>

inputHandler::inputHandler() {
  this->isUrl = false;
  this->setInput("");
  this->localPath = CLONE_TO;
  this->inputStrategy = nullptr;
  this->setProcessSuccess(false);
}

void inputHandler::pickStrategy(std::string input) {

  if (this->argumentChecker(input) == false) {
    std::cerr << "error in input handler, argument invalid"
              << std::endl; // no no to cout in member function? repalce with
                            // try catch (?)
    this->inputStrategy = nullptr;
  } else {

    if (this->isUrl == true) {
      // It was a valid URL input
      this->inputStrategy = new typeURL(this);
    } else {
      this->inputStrategy = new typeFolder();
    }
  }

  this->setInput(input);
}

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
  /*
  else if (//Condition)
  {

      this->isUrl = false;
      // if folder it's not a file path or an url break

      // if it's a filepath make sure it leads to a gitRepo
  }

  */
  else {
    // It's neither folder or URL so invalid input

    return false;
  }
}

inputHandler::~inputHandler() { delete this->inputStrategy; }

bool inputHandler::getIsUrl() { return this->isUrl; }

void inputHandler::setInput(std::string in) { this->input = in; }

void inputHandler::setProcessSuccess(bool flag) { this->processSuccess = flag; }

void inputHandler::executeStrategy() { inputStrategy->proccessInput(); }

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

  if (int err = git_clone(&repo, url.c_str(), localPath.c_str(), NULL) != 0) {
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

std::string typeFolder::getInput() { return ""; }

void typeFolder::proccessInput() {}