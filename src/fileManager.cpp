#include "fileManager.h"
#include <fstream>
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

void fileManager::mkdir(std::string path) {

  if (!std::filesystem::create_directory(path)) {
    std::cerr << "Failed to create directory " + path << std::endl;
  }
}

void fileManager::checkAndClear(std::string path) {

  if (!this->dirExists(path)) {

    this->mkdir(path);
    return;
  }

  if (this->isEmpty(path) != true) {

    this->clearDir(path);
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

bool fileManager::checkIsDir(std::string path) {

  std::filesystem::path fsPath = this->stringToPath(path);

  if (std::filesystem::is_directory(fsPath)) {
    return true;
  }

  return false;
}

std::filesystem::path fileManager::stringToPath(std::string str) {

  std::filesystem::path path = str;
  return path;
}

bool fileManager::checkContentsIsEmpty(std::string path) {
  // Determine if its a dir

  if (this->checkIsDir(path)) {

    // Do dir checkContents
    return this->dirIsEmpty(path);
  }

  // Else do file checkContetns
  return fileIsEmpty(path);
}

bool fileManager::dirIsEmpty(std::string path) {

  auto it = std::filesystem::directory_iterator(path);
  if (it != std::filesystem::directory_iterator(path)) {
    // dir is not empty
    return false;
  } else {
    return true;
  }
}

bool fileManager::fileIsEmpty(std::string path) {

  std::ifstream file(path);

  return file.peek() == std::ifstream::traits_type::eof();
}

std::string fileManager::copyRepoToInternal(const std::string &srcPath) {
  std::filesystem::path src(srcPath);
  std::filesystem::path dest("../tmp/localRepo");
  try {
    // Remove any previous copy
    if (std::filesystem::exists(dest)) {
      std::filesystem::remove_all(dest);
    }
    // Create the destination directory
    std::filesystem::create_directories(dest);
    // Recursively copy the directory
    std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << "\n";
    return "";
  }
  return dest.string();
}