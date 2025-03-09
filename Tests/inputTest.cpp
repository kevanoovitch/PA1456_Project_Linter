
#include "constants.h"
#include "fileManager.h"
#include "inputHandler.h"
#include "testArgs.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace TestArgs;
using namespace constants;

TEST(fsBasics, memberTest) {

  fileManager filesys;
  std::ofstream file;

  file.open(TESTDIR_PATH + "/test.txt");
  file.close();
  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), false) << "should not be empty";

  filesys.clearDir(TESTDIR_PATH);

  ASSERT_EQ(filesys.isEmpty(TESTDIR_PATH), true);

  file.open(TESTDIR_PATH + "/test.txt");
  file.close();

  filesys.checkAndClear(TESTDIR_PATH);
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
  fileManager filesys;
  handler1.pickStrategy(URL_REPO_SMALL_SIZE);

  ASSERT_EQ(typeid(*handler1.inputStrategy), typeid(typeURL))
      << "incorrect strategy picked";

  if (filesys.isEmpty(constants::REPOSITORY_PATH) != true) {

    filesys.clearDir(constants::REPOSITORY_PATH);
  }

  handler1.executeStrategy();

  ASSERT_EQ(handler1.processSuccess, true)
      << "Failed to process input using URL (Was /Repository not empty?)";
}

TEST(typeFolder, folderHandling) {
  inputHandler handler1;
  fileManager filesys;
  handler1.pickStrategy(FOLDER_ABSOLUTE_REPO_TIMER_APP);

  ASSERT_EQ(typeid(*(handler1.inputStrategy)),
            typeid(typeFolder)) // returns true if x,x are equal type
      << "Expected inputStrategy to be of typeFolder, but it wasn't";

  handler1.executeStrategy();

  ASSERT_EQ(handler1.localPath, REPOSITORY_PATH)
      << "Expected folderpath to have switched from default";
}
