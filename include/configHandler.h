#pragma once

#include "constants.h"
#include "errorStatus.h"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
using namespace constants;

struct fileParams {

  std::unordered_map<std::string, bool> properties;
};

struct config {
  static std::string relRepoPath;
  static std::string relWorkflowPath;
  static bool minimalOutputMode;

  static std::unordered_map<std::string, fileParams> fileReqs;
};

class configHandler {
private:
  nlohmann::json jFile;
  template <typename T> void readConfig(std::string key, T &targetValue);
  void readDstPath();
  void readOutputMode();
  void readIndicationParams();
  void openAndSetConfigFile(std::string path);
  void readFileReq(std::string file, std::string key, bool &targetValue);

public:
  configHandler(/* args */);
  ~configHandler();
  void configure(std::string path = PATH_CONFIG);
};
