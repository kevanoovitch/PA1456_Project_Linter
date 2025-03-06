#include "configHandler.h"
#include "constants.h"
#include "scanner.h"
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace constants;

TEST(ConfigTest, NewRepoFolder) {

  // Set the repo folder and check that it did it correctly

  configHandler config;

  std::ifstream file(PATH_CONFIG);
  nlohmann::json configFile;
  file >> configFile;
  std::string newPath;

  if (!configFile.empty()) {

    newPath = configFile["repoPath"];
  }

  config.readConfigFile();

  fileManager filesys;

  bool existsResult = filesys.dirExists(newPath);

  EXPECT_EQ(existsResult, true)
      << "The folder according to the config should've been created";
}