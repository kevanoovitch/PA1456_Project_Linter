#include "constants.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include "scanner.h"
#include "testArgs.h"

#include <git2.h>

using namespace constants;
using namespace CommonSearchTerms;

int main() {
  git_libgit2_init();

  std::string userInput;
  inputHandler userInputHandler;
  fileManager filesys;

  std::cout << "Welcome to the Linter prototype" << std::endl;
  std::cout << "Please input an URL or a absolute Path to a GitHub repositoy "
               "you want to analyze"
            << std::endl;
  std::cin >> userInput;

  // interpret input

  userInputHandler.pickStrategy(userInput);

  Strategy *ptr = userInputHandler.getStrategyPtr();
  if (!ptr) {
    std::cerr << "❌ Error: Strategy pointer is null. Aborting.\n";
    return 1; // Exit or handle error
  }

  userInputHandler.executeStrategy();

  // Scanning

  Scanner theScanner(userInputHandler);

  resultInterpreter theResult(userInputHandler);

  theScanner.scanFor(gitIgnoreAlts, GIT_IGNORE);

  theScanner.scanForWorkflow();

  theScanner.scanFor(licenseAlts, LICENSE);

  theScanner.scanFor(readmeAlts, README);

  theScanner.scanForTest(testAlts, TEST_STRING);

  theScanner.scanGitAttributes();

  theScanner.runGitLeaks();

  // Result

  theResult.interpretResults();

  theResult.printDetails();

  return 0;
}