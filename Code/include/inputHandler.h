#pragma once
#include <git2.h>
#include <gtest/gtest.h>
#include <string>

const std::string CLONE_TO = "../Repository";

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Strategy; // Forward declare Strategy since we just need a pointer in
                // inputHandler

/**********************************************************
 *                  InputHandler                          *
 **********************************************************/

class inputHandler {

public:
  inputHandler();
  ~inputHandler();
  bool getIsUrl();
  void setInput(std::string const in);
  git_repository *repo;
  void setProcessSuccess(bool flag);
  void pickStrategy(std::string input);
  void executeStrategy();
  std::string getInput();

protected:
  std::string localPath;

private:
  bool argumentChecker(std::string arg);
  bool processSuccess;
  bool isUrl;
  std::string input;
  Strategy *inputStrategy; // a pointer for what type of child it is

  /*Test related*/
  FRIEND_TEST(inputHandler,
              inputArgsTest); // Allow specific test to access private members
  FRIEND_TEST(typeURL,
              URLhandling); // Allow specific test to access private members
};

/**********************************************************
 *                  Strategy                              *
 **********************************************************/

class Strategy : public inputHandler {

public:
  virtual std::string getInput() = 0;
  virtual ~Strategy();
  virtual void proccessInput() = 0;
};

/**********************************************************
 *                  typeURL                               *
 **********************************************************/

class typeURL : public Strategy {
public:
  typeURL() = default;
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
  typeFolder() = default;
  ~typeFolder();

  std::string getInput() override;
  void proccessInput() override;

private:
  inputHandler *parentInputHandler;
};
