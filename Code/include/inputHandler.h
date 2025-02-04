#pragma once
#include <string>
#include <gtest/gtest.h>

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/

class Strategy;  // Forward declare Strategy since we just need a pointer in inputHandler


/**********************************************************
 *                  InputHandler                          *
 **********************************************************/

class inputHandler
{
private:
    /* data */
    bool argumentChecker(std::string arg);
    FRIEND_TEST(inputHandler, inputArgsTest); // Allow specific test to access private members

    bool isUrl;

    Strategy *inputStrategy; // a pointer for what type of child it is

public:
    inputHandler(std::string input); // Kalla argumentChecker och sätt isvalid
    inputHandler();
    ~inputHandler();
    bool getIsUrl();
    void setInput(std::string const in);

protected:
    std::string input;
};

/**********************************************************
 *                  Strategy                              *
 **********************************************************/

class Strategy : public inputHandler
{

public:
  virtual std::string getInput() = 0;
  virtual ~Strategy();
};

/**********************************************************
 *                  typeURL                               *
 **********************************************************/

class typeURL : public Strategy
{
public:
    typeURL();
    ~typeURL();
    // bool getIsUrl() override;
    std::string getInput() override;
};

/**********************************************************
 *                  typeFolder                            *
 **********************************************************/
class typeFolder : public Strategy
{
public:
    typeFolder();
    ~typeFolder();
    // bool getIsUrl() override;
    std::string getInput() override;
};
