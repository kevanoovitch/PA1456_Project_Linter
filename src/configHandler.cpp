#include "configHandler.h"
#include "constants.h"
#include "fileManager.h"
#include <fstream>
#include <iostream>

using namespace constants;

// Static members: Config
std::string config::relRepoPath = "";
std::string config::relWorkflowPath = "";
std::unordered_map<std::string, fileParams> config::fileReqs;
bool config::minimalOutputMode = false;

// Static members: Errorstatus
int errorStatus::statusInt = 0;
std::string errorStatus::statusMessage = NIL;

configHandler::configHandler(/* args */) {}

configHandler::~configHandler() {}

void configHandler::openAndSetConfigFile(std::string path) {
  std::ifstream file(path);
  if (!file) {
    std::cerr << "Failed to open config JSON file: " + path << std::endl;
    errorStatus::statusInt = 2;
    return;
  }

  nlohmann::json configFile;
  file >> configFile;

  this->jFile = configFile;
}
template <typename T>
void configHandler::readConfig(std::string key, T &targetValue) {
  // Target value is a refrence to the config struct

  if (jFile["custom"].contains(key) && !jFile["custom"][key].is_null()) {
    targetValue = jFile["custom"][key].get<T>();
  } else {
    targetValue = jFile["defaults"][key].get<T>();
  }
}

void configHandler::readFileReq(std::string file, std::string key,
                                bool &targetValue) {
  // Target value is a refrence to the config struct

  if (this->jFile["custom"][file][key].is_null()) {
    targetValue = this->jFile["defaults"][file][key].get<bool>();
  } else {
    targetValue = this->jFile["custom"][file][key].get<bool>();
    ;
  }
}

void configHandler::readOutputMode() {

  readConfig("minimalOutput", config::minimalOutputMode);
}

void configHandler::configure(std::string path) {

  openAndSetConfigFile(path);

  this->readDstPath();
  this->readIndicationParams();
  this->readOutputMode();
}

void configHandler::readDstPath() {

  readConfig("repoPath", config::relRepoPath);

  config::relWorkflowPath = config::relRepoPath + "/.github/workflows";

  constants::REPOSITORY_PATH = config::relRepoPath;
  constants::WORKFLOW_PATH = config::relWorkflowPath;

  fileManager filesys;

  filesys.ensureFolderExists(config::relRepoPath);
}

void fileManager::ensureFolderExists(std::string path) {

  if (!this->dirExists(path)) {
    // if the folder doesn't exist, create it
    this->mkdir(path);
  }
}

void configHandler::readIndicationParams() {

  std::vector<std::string> files = {README, LICENSE, GIT_IGNORE, TEST_STRING,
                                    WORKFLOW_STRING};

  std::vector<std::string> keys = {"required", "requiresContent",
                                   "allowMultiple"};

  for (auto file : files) {

    for (auto key : keys) {
      this->readFileReq(file, key, config::fileReqs[file].properties[key]);
    }
  }
}
