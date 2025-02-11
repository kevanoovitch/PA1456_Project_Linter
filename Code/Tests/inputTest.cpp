
#include "fs.h"
#include "inputHandler.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <typeinfo>

namespace TestArgs {
const std::string URL_REPO_FULL =
    "https://github.com/kevanoovitch/-DV1697-OS-course";
const std::string URL_NON_REPO = "https://www.dn.se/";
const std::string URL_REPO_MINIMAL =
    "github.com/kevanoovitch/-DV1697-OS-course";
const std::string URL_REPO_SMALL_SIZE =
    "https://github.com/kevanoovitch/TimerApplication";
const std::string REPOSITORY_PATH = "../Repository";
const std::string TESTDIR_PATH = "../Tests/testDir";

} // namespace TestArgs

using namespace TestArgs;

TEST(fsBasics, memberTest) {

  fs filesys;
  std::ofstream file;

  file.open(TESTDIR_PATH + "/test.txt");
  file.close();
  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), false);

  filesys.clearDir(TESTDIR_PATH);

  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), true);

  file.open(TESTDIR_PATH + "/test.txt");
  file.close();

  std::istringstream simulatedNo("n");

  filesys.checkAndClear(TESTDIR_PATH, simulatedNo);

  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), false);

  std::istringstream simulatedYes("y");
  filesys.checkAndClear(TESTDIR_PATH, simulatedYes);
  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), true);
}

TEST(inputHandler, inputArgsTest) {

  /*test that a correct url works*/

  inputHandler handler1;

  handler1.pickStrategy(URL_REPO_FULL);

  ASSERT_EQ(handler1.argumentChecker(URL_REPO_FULL), true)
      << "Expected argChecker to return true on okay URL, but it returned "
         "false";

  ASSERT_EQ(typeid(*(handler1.inputStrategy)),
            typeid(typeURL)) // returns true if x,x are equal type
      << "Expected inputStrategy to be of typeURL, but it wasn't";

  /*--- Test: non-git url gets denied ---*/

  inputHandler handler2;
  handler2.pickStrategy(URL_NON_REPO);

  ASSERT_EQ(handler2.argumentChecker(URL_NON_REPO), false)
      << "Expected argChecker to return false on invalid URL, but it returned "
         "true";

  ASSERT_EQ(handler2.inputStrategy, nullptr)
      << "Expected inputStrategy to be null ptr when invalid URL was inputed, "
         "but it wasn't";

  /* Test minimal link works*/

  inputHandler handler3;
  handler3.pickStrategy(URL_REPO_MINIMAL);

  ASSERT_EQ(handler3.argumentChecker(URL_REPO_MINIMAL), true)
      << "Expected argChecker to return true on okay URL, but it returned "
         "false";

  ASSERT_EQ(typeid(*(handler3.inputStrategy)), typeid(typeURL))
      << "Expected inputStrategy to be nullptr when invalid URL was inputed, "
         "but it wasn't";
}

TEST(typeURL, URLhandling) {

  inputHandler handler1;
  fs filesys;
  handler1.pickStrategy(URL_REPO_SMALL_SIZE);

  ASSERT_EQ(typeid(*handler1.inputStrategy), typeid(typeURL))
      << "incorrect strategy picked";

  if (filesys.isEmpty(REPOSITORY_PATH) != true) {

    filesys.clearDir(REPOSITORY_PATH);
  }

  handler1.executeStrategy();

  if (git_error_last != nullptr) {
    std::cout << "\n";
    std::cout << "GITLIB ERROR: " << git_error_last()->message << std::endl;
    std::cout << "\n";
  }

  ASSERT_EQ(handler1.processSuccess, true)
      << "Failed to process input using URL (Was /Repository not empty?)";
}

int main(int argc, char **argv) {
  git_libgit2_init();
  testing::InitGoogleTest(&argc, argv);

  int testResults = RUN_ALL_TESTS();

  return testResults;

  /*--- Test 2: test if all files are listed ---*/
}

/*--- Test 2: test if all files are listed ---*/
