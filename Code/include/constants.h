#pragma once

#include <string>
#include <vector>
namespace constants {
extern const std::string GIT_IGNORE;
extern const std::string WORKFLOW_STRING;
extern const std::string LICENSE;
extern const std::string REPOSITORY_PATH;
extern const std::string WORKFLOW_PATH;
extern const std::string README;

} // namespace constants

namespace CommonSearchTerms {
extern std::vector<std::string> gitIgnoreAlts;
extern std::vector<std::string> licenseAlts;
extern std::vector<std::string> readmeAlts;

} // namespace CommonSearchTerms
