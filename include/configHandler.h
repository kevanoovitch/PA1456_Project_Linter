
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct config {
  static std::string relRepoPath;
  static std::string relWorkflowPath;

  // std::unordered_map<std::string, bool> readmeReqAndStatus;
};

class configHandler {
private:
  nlohmann::json jFile;

  void readConfig(std::string key, std::string &targetValue);
  void readDstPath();
  void readIndicationParams();
  void openAndSetConfigFile();

public:
  configHandler(/* args */);
  ~configHandler();
  void configure();
};
