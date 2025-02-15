#include "constants.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include "scanner.h"
#include <git2.h>

using namespace constants;
using namespace CommonSearchTerms;

// Will call all the functions needed

int main() {
  git_libgit2_init();

  std::string userInput;
  inputHandler userInputHandler;
  fileManager filesys;

  std::cout << "Welcome to the Linter prototype" << std::endl;
  std::cout << "Please input an URL to github repositoy you want to analyze"
            << std::endl;
  std::cin >> userInput;

  // interpret input

  userInputHandler.pickStrategy(userInput);

  if (userInputHandler.getIsUrl() == true) {
    filesys.checkAndClear(REPOSITORY_PATH);
  }

  userInputHandler.executeStrategy();

  // Scanning
  std::cout << "cloning done, procceeding to scan" << std::endl;

  Scanner theScanner;
  resultInterpreter theResult(theScanner.myResults);

  theScanner.scanFor(gitIgnoreAlts, GIT_IGNORE);

  theScanner.scanForWorkflow();

  theScanner.scanFor(licenseAlts, LICENSE);

  theScanner.scanFor(readmeAlts, README);

  // Result
  theResult.printResults();

  theResult.printDetails();

  return 0;
}