#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <string>

class fileManager {

public:
  void checkAndClear(std::string path, std::istream &in = std::cin);
  void clearDir(std::string path);
  bool isEmpty(std::string path);
  bool dirExists(std::string path);
  bool checkValidRepoPath(std::string path);
};