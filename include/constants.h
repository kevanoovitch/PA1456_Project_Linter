#pragma once

#include <string>
#include <vector>
namespace constants {
extern const std::string GIT_IGNORE;
extern const std::string WORKFLOW_STRING;
extern const std::string LICENSE;
extern std::string REPOSITORY_PATH;
extern std::string WORKFLOW_PATH;
extern const std::string README;
extern const std::string TEST_STRING;
extern const std::string LEAK_STRING;

extern const std::string RED;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string WHITE;

extern const std::string LINK_LICENSE;
extern const std::string LINK_README;
extern const std::string LINK_IGNORE;
extern const std::string LINK_TESTS;
extern const std::string LINK_WORKFLOW;

extern const std::string NIL;
extern const std::string NO_CHECKS;

extern const std::string PATH_REPORT_CREDENTIALS;
extern const std::string PATH_CONFIG;

} // namespace constants

namespace CommonSearchTerms {
extern std::vector<std::string> gitIgnoreAlts;
extern std::vector<std::string> licenseAlts;
extern std::vector<std::string> readmeAlts;
extern std::vector<std::string> testAlts;

} // namespace CommonSearchTerms
