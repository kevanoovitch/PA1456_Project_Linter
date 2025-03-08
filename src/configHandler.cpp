#include "configHandler.h"
#include "constants.h"
#include "fileManager.h"
#include <fstream>

using namespace constants;

std::string config::relRepoPath = "";
std::string config::relWorkflowPath = "";

configHandler::configHandler(/* args */) {}

configHandler::~configHandler() {}

void configHandler::openAndSetConfigFile() {
  std::ifstream file(PATH_CONFIG);
  if (!file) {
    std::cerr << "Failed to open config JSON output." << std::endl;
    return;
  }

  nlohmann::json configFile;
  file >> configFile;

  this->jFile = configFile;
}

void configHandler::readConfig(std::string key, std::string &targetValue) {
  // Target value is a refrence to the config struct

  std::string tmp = this->jFile["custom"][key];

  if (tmp.empty() == true) {
    targetValue = this->jFile["defaults"][key];
  } else {
    targetValue = tmp;
  }
}

void configHandler::configure() {

  openAndSetConfigFile();

  this->readDstPath();
  this->readIndicationParams();
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

void configHandler::readIndicationParams() { return; }
