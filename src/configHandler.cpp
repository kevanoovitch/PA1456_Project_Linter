#include "configHandler.h"
#include "constants.h"
#include "fileManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

using namespace constants;

configHandler::configHandler(/* args */) {}

configHandler::~configHandler() {}

void configHandler::readConfigFile() { this->readDstPath(); }

void configHandler::readDstPath() {

  std::ifstream file(PATH_CONFIG);
  if (!file) {
    std::cerr << "Failed to open config JSON output." << std::endl;
    return;
  }

  nlohmann::json configFile;
  file >> configFile;

  if (!configFile.empty()) {

    constants::REPOSITORY_PATH = configFile["repoPath"];
    constants::WORKFLOW_PATH = REPOSITORY_PATH + "/.github/workflows";
  }
  fileManager filesys;
  filesys.ensureFolderExists(REPOSITORY_PATH);
}

void fileManager::ensureFolderExists(std::string path) {

  if (!this->dirExists(path)) {

    this->mkdir(path);
  }
}
