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
  handler.pickStrategy(URL_REPO_WITH_WORKFLOW);
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

  // Test above test with a git repo that lacks most!
}