#include "configHandler.h"
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
  configHandler config;

  void SetUp() override {

    // Read the configFile
    config.configure();

    // Reset file system before each test
    filesys.clearDir(REPOSITORY_PATH);

    // Get test parameter (different strategy input)
    std::string strategyInput = GetParam();

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
    theScanner->scanFor(testAlts, TEST_STRING);
    theScanner->scanForTest(testAlts, TEST_STRING);
    theScanner->scanGitAttributes();
    theScanner->runGitLeaks();

    // Interpret results
    theResult = new resultInterpreter(handler);

    theResult->interpretResults();
  }

  void TearDown() override {
    delete theScanner;
    delete theResult;
  }
};

class LinterTestPasses : public LinterTest {}; // Test fixture for passing cases
class LinterTestFails : public LinterTest {};  // Test fixture for failing cases

INSTANTIATE_TEST_SUITE_P(PassesAllTest, LinterTestPasses,
                         ::testing::Values(FOLDER_DUMMY_ALL));

INSTANTIATE_TEST_SUITE_P(FailsAllTest, LinterTestFails,
                         ::testing::Values(FOLDER_DUMMY_NONE));

TEST_P(LinterTestPasses, PassesAll) {
  std::string useCase = GetParam();
  std::cout << "[PassesAll] Testing use case: " << useCase << std::endl;

  for (auto &it : theResult->AllResultEntries) {
    EXPECT_EQ(it->Indication, GREEN)
        << "❌ [PassesAll] Failed for entry: " << it->entryName << "\n"
        << "Reason: " << it->IndicationReason;
  }
}

TEST_P(LinterTestFails, FailsAllTest) {
  std::string useCase = GetParam();
  std::cout << "[FailsAll] Testing use case: " << useCase << std::endl;

  for (auto &it : theResult->AllResultEntries) {
    EXPECT_EQ(it->Indication, RED)
        << "❌ [FailsAll] Failed for entry: " << it->entryName << "\n"
        << "Reason: " << it->IndicationReason;
  }
}
