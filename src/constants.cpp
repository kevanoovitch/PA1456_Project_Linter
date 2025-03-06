#include "constants.h"

namespace constants {
const std::string GIT_IGNORE = ".gitignore";
const std::string WORKFLOW_STRING = "Workflow";
const std::string LICENSE = "License";
const std::string REPOSITORY_PATH = "../Repository";
const std::string WORKFLOW_PATH = "../Repository/.github/workflows";
const std::string README = "readMe";
const std::string TEST_STRING = "tests";

const std::string RED = "\U0001F534";
const std::string GREEN = "\U0001F7E2";
const std::string YELLOW = "\U0001F7E1";
const std::string WHITE = "\u26AA";

extern const std::string PATH_REPORT_CREDENTIALS =
    "../Repository/gitleaksReport.json";

const std::string linkLicense = "https://choosealicense.com/";
const std::string linkReadme =
    "https://docs.github.com/en/repositories/"
    "managing-your-repositorys-settings-and-features/"
    "customizing-your-repository/about-readmes";
const std::string linkIgnore = "https://github.com/github/gitignore";
const std::string linkTests = "https://learn.microsoft.com/en-us/dotnet/core/"
                              "testing/unit-testing-best-practices";
const std::string linkWorkflow =
    "https://docs.github.com/en/actions/writing-workflows";

const std::string NIL = "No issues detected";

} // namespace constants

namespace CommonSearchTerms {
std::vector<std::string> gitIgnoreAlts = {constants::GIT_IGNORE};

std::vector<std::string> licenseAlts = {"license", "license.txt", "unlicense",
                                        "unlicense.txt", "copying"};
std::vector<std::string> readmeAlts = {"readme", "readme.md"};

std::vector<std::string> testAlts = {"test", "tests"};

} // namespace CommonSearchTerms