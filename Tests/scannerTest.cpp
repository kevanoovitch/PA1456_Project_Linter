#include "scanner.h"
#include "constants.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include "testArgs.h"
#include <gtest/gtest.h>
using namespace TestArgs;
using namespace constants;
using namespace CommonSearchTerms;

TEST(ScannerOperations, findRequiredFiles) {

  // Setup Test Environment

  inputHandler handler;
  fileManager filesys;

  filesys.clearDir(REPOSITORY_PATH);
  handler.pickStrategy(FOLDER_DUMMY_ALL);
  handler.executeStrategy();

  // Execute Tests
  Scanner myScanner(handler);
  resultInterpreter interpret(handler);
  bool searchResult = false;

  searchResult = interpret.isFound(GIT_IGNORE);

  ASSERT_EQ(searchResult, false)
      << "Checked result before searching for .gitignore";

  myScanner.scanFor(gitIgnoreAlts, GIT_IGNORE);

  searchResult = interpret.isFound(GIT_IGNORE);

  ASSERT_EQ(searchResult, true)
      << "Didn't find the gitIgnore file when it was present";

  // Workflow tests

  bool existResult = myScanner.fileManagerPtr->dirExists(WORKFLOW_PATH);

  ASSERT_EQ(existResult, true)
      << "Expected there to be a workflow dir but there wasn't";

  myScanner.scanForWorkflow();

  bool result = interpret.isFound(WORKFLOW_STRING);
  ASSERT_EQ(result, true) << "Expected there were either .yml or .yaml files "
                             "under the /workflows but there wasn't";

  // License file tests

  myScanner.scanFor(licenseAlts, LICENSE);

  result = interpret.isFound(LICENSE);
  ASSERT_EQ(result, true) << "Expected to find a license but didn't";

  // readme file test

  myScanner.scanFor(readmeAlts, README);
  result = interpret.isFound(README);
  ASSERT_EQ(result, true) << "Expected to find a readme but didn't";
}

TEST(ScannerOperations, dontfindRequiredFiles) {

  // Setup Test Environment

  inputHandler handler;
  fileManager filesys;

  filesys.clearDir(REPOSITORY_PATH);

  handler.pickStrategy(FOLDER_DUMMY_NONE);
  handler.executeStrategy();

  // Execute Tests
  Scanner myScanner(handler);
  resultInterpreter interpret(handler);
  bool searchResult = false;

  // gitIgnore

  myScanner.scanFor(gitIgnoreAlts, GIT_IGNORE);

  searchResult = interpret.isFound(GIT_IGNORE);

  ASSERT_EQ(searchResult, false)
      << "Found gitIgnore when it wasn't or shouln't be present";

  // Workflow tests

  bool existResult = myScanner.fileManagerPtr->dirExists(WORKFLOW_PATH);

  ASSERT_EQ(existResult, false)
      << "Expected there NOT to be a workflow dir but there wasn't";

  myScanner.scanForWorkflow();

  bool result = interpret.isFound(WORKFLOW_STRING);
  ASSERT_EQ(result, false)
      << "Found workflow files when it wasn't or shouln't be present";

  // License file tests

  myScanner.scanFor(licenseAlts, LICENSE);

  result = interpret.isFound(LICENSE);
  ASSERT_EQ(result, false)
      << "Found License when it wasn't or shouln't be present";

  // Read me file test

  myScanner.scanFor(readmeAlts, README);
  result = interpret.isFound(README);
  ASSERT_EQ(result, false) << "Expected to NOT find a readme but did";

  //'Tests' test

  myScanner.scanFor(testAlts, TEST_STRING);

  result = interpret.isFound(TEST_STRING);
  EXPECT_EQ(result, false);
}

TEST(ScannerGitOperations, findGitAtributes) {

  // Setup Test Environment

  inputHandler handler;
  fileManager filesys;

  filesys.clearDir(REPOSITORY_PATH);
  handler.pickStrategy(URL_REPO_SMALL_SIZE);
  handler.executeStrategy();

  // Execute Tests
  Scanner myScanner(handler);

  resultInterpreter interpret(handler);
  int commitResult = 0;

  // Count commits - friend function

  commitResult =
      myScanner.myGitScanner->countCommits(myScanner.sharedResult->repo);

  EXPECT_GT(commitResult, 0)
      << "Expected commit count to be bigger than 1 but it wasn't";

  // List contributors -friend function

  std::unordered_map<std::string, int> contributorsMap =
      myScanner.myGitScanner->countContributors(myScanner.sharedResult->repo);

  int nrOfContributors = contributorsMap.size();

  EXPECT_GT(nrOfContributors, 0)
      << "Expected commit count to be bigger than 1 but it wasn't";

  // List and count git attributes and write to result - proper method
  myScanner.scanGitAttributes();

  EXPECT_GT(myScanner.sharedResult->resultNrOfCommits, 0);
  EXPECT_GT(myScanner.sharedResult->contributorCounts.size(), 0);
}
