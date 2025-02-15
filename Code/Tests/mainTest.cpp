#include <git2.h>
#include <gtest/gtest.h>

namespace TestArgs {
const std::string URL_REPO_FULL =
    "https://github.com/kevanoovitch/-DV1697-OS-course";
const std::string URL_NON_REPO = "https://www.dn.se/";
const std::string URL_REPO_MINIMAL =
    "github.com/kevanoovitch/-DV1697-OS-course";
const std::string URL_REPO_SMALL_SIZE =
    "https://github.com/kevanoovitch/TimerApplication";

const std::string TESTDIR_PATH = "../Tests/testDir";

} // namespace TestArgs

int main(int argc, char **argv) {
  git_libgit2_init();
  testing::InitGoogleTest(&argc, argv);

  int testResults = RUN_ALL_TESTS();

  return testResults;
}