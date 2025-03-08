
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct fileParams {

  std::unordered_map<std::string, bool> properties;
};

struct config {
  static std::string relRepoPath;
  static std::string relWorkflowPath;

  static std::unordered_map<std::string, fileParams> fileReqs;
};

class configHandler {
private:
  nlohmann::json jFile;

  void readConfig(std::string key, std::string &targetValue);
  void readDstPath();
  void readIndicationParams();
  void openAndSetConfigFile();
  void readFileReq(std::string file, std::string key, bool &targetValue);

public:
  configHandler(/* args */);
  ~configHandler();
  void configure();
};
