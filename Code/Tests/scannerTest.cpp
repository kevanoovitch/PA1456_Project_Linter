#include "scanner.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "resultInterpreter.h"
#include "testArgs.h"
#include <gtest/gtest.h>
using namespace TestArgs;

TEST(ScannerOperations, findRequiredFiles) {

  // Setup Test Environment

  inputHandler handler;
  fileManager filesys;

  filesys.clearDir(REPOSITORY_PATH);
  handler.pickStrategy(URL_REPO_WITH_ALL);
  handler.executeStrategy();

  // Execute Tests
  Scanner myScanner;
  resultInterpreter interpret(myScanner.myResults);
  bool searchResult = false;

  searchResult = interpret.isFound(".gitignore");

  ASSERT_EQ(searchResult, false)
      << "Checked result before searching for .gitignore";

  myScanner.scanForGitignore();

  searchResult = interpret.isFound(".gitignore");

  ASSERT_EQ(searchResult, true)
      << "Didn't find the gitIgnore file when it was present";

  // Workflow tests

  bool existResult = myScanner.fileManagerPtr->dirExists(WORKFLOW_PATH);

  ASSERT_EQ(existResult, true)
      << "Expected there to be a workflow dir but there wasn't";

  myScanner.scanForWorkflow();

  bool result = interpret.isFound("Workflow");
  ASSERT_EQ(result, true) << "Expected there were either .yml or .yaml files "
                             "under the /workflows but there wasn't";

  // License file tests

  myScanner.scanForLicense();

  result = interpret.isFound("License");
  ASSERT_EQ(result, false) << "Expected to find a license but didn't";
}

TEST(ScannerOperations, dontfindRequiredFiles) {

  // Setup Test Environment

  inputHandler handler;
  fileManager filesys;

  filesys.clearDir(REPOSITORY_PATH);
  handler.pickStrategy(URL_REPO_SMALL_SIZE);
  handler.executeStrategy();

  // Execute Tests
  Scanner myScanner;
  resultInterpreter interpret(myScanner.myResults);
  bool searchResult = false;

  // gitIgnore

  myScanner.scanForGitignore();

  searchResult = interpret.isFound(".gitignore");

  ASSERT_EQ(searchResult, false)
      << "Found gitIgnore when it wasn't or shouln't be present";

  // Workflow tests

  bool existResult = myScanner.fileManagerPtr->dirExists(WORKFLOW_PATH);

  ASSERT_EQ(existResult, false)
      << "Expected there NOT to be a workflow dir but there wasn't";

  myScanner.scanForWorkflow();

  bool result = interpret.isFound("Workflow");
  ASSERT_EQ(result, false)
      << "Found workflow files when it wasn't or shouln't be present";

  // License file tests

  myScanner.scanForLicense();

  result = interpret.isFound("License");
  ASSERT_EQ(result, false)
      << "Found License when it wasn't or shouln't be present";
}
