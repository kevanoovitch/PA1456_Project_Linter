#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <string>

class fileManager {

public:
  void checkAndClear(std::string path);
  void clearDir(std::string path);
  bool isEmpty(std::string path);
  bool dirExists(std::string path);
  bool checkValidRepoPath(std::string path);
  void mkdir(std::string path);

  bool checkContentsIsEmpty(std::string path);
  void ensureFolderExists(std::string path);
  std::string copyRepoToInternal(const std::string &srcPath);

private:
  std::filesystem::path stringToPath(std::string string);
  bool checkIsDir(std::string path);
  bool dirIsEmpty(std::string path);
  bool fileIsEmpty(std::string path);
};