#include "configHandler.h"
#include "constants.h"
#include "scanner.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace constants;

TEST(ConfigTest, NewRepoFolder) {

  // Set the repo folder and check that it did it correctly

  configHandler config;

  std::string newPath;

  config.configure();

  newPath = REPOSITORY_PATH;

  fileManager filesys;

  EXPECT_EQ(config::relRepoPath, REPOSITORY_PATH)
      << "The repo path should be set to the default path";

  bool existsResult = filesys.dirExists(newPath);

  EXPECT_EQ(existsResult, true)
      << "The folder according to the config should've been created";
}