#include "inputHandler.h"
#include <string>
#include <regex>

inputHandler::inputHandler()
{
    this->isUrl = false;
    this->input = "";
    this->inputStrategy = nullptr;
}

inputHandler::inputHandler(std::string input)
{
    this->isUrl = false; // assumes it's a folderpath
    this->inputStrategy = nullptr;
    this->setInput(input);

    if (this->argumentChecker(input) == false)
    {
        std::cerr << "error in input handler" << std::endl; // no no to cout in member function? repalce with try catch
        this->inputStrategy = nullptr;
    }
    else
    {

        if (this->isUrl == true)
        {
            // It was a valid URL input
            this->inputStrategy = new typeURL();
        }
        else
        {
            this->inputStrategy = new typeFolder();
        }
    }
}

bool inputHandler::argumentChecker(std::string arg)
{
    /*Return true if valid, return false if unvalid*/

    // Regex to check valid URL

    const std::regex pattern(R"((https:\/\/)?github\.com\/[a-zA-Z0-9_\-]+\/[a-zA-Z0-9_\-\.]+)");

    //(https?:\/\/github\.com\/[a-zA-Z0-9_\-]+\/[a-zA-Z0-9_\-\.]+) //this does not allow minimal links!

    // If the URL
    // is empty return false
    if (arg.empty())
    {

        return false;
    }

    // Return true if the URL
    // matched the ReGex
    if (regex_match(arg, pattern))
    {

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
    else
    {
        // It's neither folder or URL so invalid input

        return false;
    }
}

inputHandler::~inputHandler()
{
    delete this->inputStrategy;
}

bool inputHandler::getIsUrl()
{
    return this->isUrl;
}

void inputHandler::setInput(std::string in)
{
    this->input = in;
}

/**********************************************************
 *                  Strategy Implementation               *
 **********************************************************/

Strategy::~Strategy()
{
}

/**********************************************************
 *                  URL Implementation                    *
 **********************************************************/

typeURL::typeURL()
{
}

typeURL::~typeURL() {}

std::string typeURL::getInput()
{
    return this->input;
}

/**********************************************************
 *                Folder Implementation                   *
 **********************************************************/

typeFolder::typeFolder()
{
}

typeFolder::~typeFolder()
{
}

std::string typeFolder::getInput()
{
    return this->input;
}
