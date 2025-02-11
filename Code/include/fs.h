#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <string>

class fs {

public:
  void checkAndClear(std::string path, std::istream &in = std::cin);
  void clearDir(std::string path);
  bool isEmpty(std::string path);
};