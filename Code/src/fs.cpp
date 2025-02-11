#include "fs.h"
#include <iostream>

void fs::clearDir(std::string path) {
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    std::filesystem::remove_all(entry.path());
  }
}

bool fs::isEmpty(std::string path) {
  if (std::filesystem::is_empty(path) == true) {
    return true;
  }

  return false;
}

void fs::checkAndClear(std::string path, std::istream &in) {

  if (this->isEmpty(path) != true) {
    std::string response;
    std::cout << path << " this was not empty!" << std::endl;
    std::cout << "Do you want to empty this folder? (y/n)" << std::endl;
    in >> response; // in is cin by default if something was passed it will be
                    // mock input
    if (response == "y") {
      this->clearDir(path);
    }
  } else {
    // do nothing
  }
}