#include "fileManager.h"
#include <iostream>

void fileManager::clearDir(std::string path) {

  for (const auto &entry : std::filesystem::directory_iterator(path)) {

    std::filesystem::remove_all(entry.path());
  }
}

bool fileManager::isEmpty(std::string path) {
  if (std::filesystem::is_empty(path) == true) {
    return true;
  }

  return false;
}

void fileManager::checkAndClear(std::string path, std::istream &in) {

  if (this->isEmpty(path) != true) {
    std::string response;
    std::cout << path << " this was not empty!" << std::endl;
    std::cout << "Do you want to empty this folder? (y/n)" << std::endl;
    in >> response; // in is cin by default if something was passed it will be
                    // mock input
    if (response == "y") {
      this->clearDir(path);
    } else {
      std::cerr
          << "Warning /Repository not cleared and not recloned. Are you sure "
             "you are "
             "checking the correct repository? If not end the restart the "
             "program."
          << std::endl;
    }
  }
}

bool fileManager::dirExists(std::string dirPath) {
  if (std::filesystem::exists(dirPath)) {
    return true;
  }
  return false;
}

bool fileManager::checkValidRepoPath(std::string path) {
  if (!std::filesystem::exists(path)) {
    return false;
  }

  if (!std::filesystem::is_directory(path)) {
    return false;
  }

  if (!std::filesystem::exists(path + "/.git")) {
    std::cerr << "couldn't find .git in the dir: " + path << std::endl;
    return false;
  }

  return true;
}
