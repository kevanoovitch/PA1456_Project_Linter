#include "constants.h"

namespace constants {
const std::string GIT_IGNORE = ".gitignore";
const std::string WORKFLOW_STRING = "Workflow";
const std::string LICENSE = "License";
const std::string REPOSITORY_PATH = "../Repository";
const std::string WORKFLOW_PATH = "../Repository/.github/workflows";
const std::string README = "readMe";

const std::string RED = "RED";
const std::string GREEN = "GREEN";
const std::string YELLOW = "YELLOW";

} // namespace constants

namespace CommonSearchTerms {
std::vector<std::string> gitIgnoreAlts = {constants::GIT_IGNORE, "GITIGNORE"};

std::vector<std::string> licenseAlts = {"LICENSE", "LICENSE.txt", "UNLICENSE",
                                        "UNLICENSE.TXT", "COPYING"};
std::vector<std::string> readmeAlts = {"readme", "README", "README.md",
                                       "readme.md"};

} // namespace CommonSearchTerms