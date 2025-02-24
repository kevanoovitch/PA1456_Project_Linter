#include "constants.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include "scanner.h"
#include "testArgs.h"

#include <git2.h>

using namespace constants;
using namespace TestArgs;
using namespace CommonSearchTerms;

class LinterTest : public ::testing::TestWithParam<std::string> {
protected:
  inputHandler handler;
  fileManager filesys;
  Scanner *theScanner = nullptr;
  resultInterpreter *theResult = nullptr;

  void SetUp() override {
    // Reset file system before each test
    filesys.clearDir(REPOSITORY_PATH);

    // Get test parameter (different strategy input)
    std::string strategyInput = GetParam();

    std::cout << strategyInput << std::endl;
    handler.pickStrategy(strategyInput);

    Strategy *ptr = handler.getStrategyPtr();
    ASSERT_NE(ptr, nullptr) << "❌ Error: Strategy pointer is null. Aborting.";

    handler.executeStrategy();

    // Initialize scanner
    theScanner = new Scanner(handler);

    // Scan for different files
    theScanner->scanFor(gitIgnoreAlts, GIT_IGNORE);
    theScanner->scanForWorkflow();
    theScanner->scanFor(licenseAlts, LICENSE);
    theScanner->scanFor(readmeAlts, README);
    theScanner->scanGitAttributes();

    // Interpret results
    theResult = new resultInterpreter(theScanner->myResults);

    theResult->interpretResults();
    theResult->printDetails();
  }

  void TearDown() override {
    delete theScanner;
    delete theResult;
  }

  // Run the full linter process
  void runProgram() {
    theResult->interpretResults();
    theResult->printDetails();
  }
};

TEST_P(LinterTest, LinterShouldProduceOutput) {
  std::string useCase = GetParam(); // Retrieves the assigned value
  std::cout << "Testing use case: " << useCase << std::endl;

  // Capture stdout
  std::ostringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

  theResult->printDetails();

  std::cout.rdbuf(old);

  std::string output = buffer.str();

  ASSERT_FALSE(output.empty()) << "❌ No output was printed!";
}

INSTANTIATE_TEST_SUITE_P(StrategyTests, LinterTest,
                         ::testing::Values(URL_REPO_WITH_ALL, URL_REPO_MINIMAL,
                                           FOLDER_ABSOLUTE_REPO_TIMER_APP,
                                           FOLDER_ABSOLUTE_WITH_ALL));
