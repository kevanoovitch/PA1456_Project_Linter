#include "inputHandler.h"
#include <git2.h>

// Will call all the functions needed

int main() {
  git_libgit2_init();

  std::string userInput;
  inputHandler userInputHandler;

  std::cout << "Welcome to the Linter protype" << std::endl;
  std::cout << "Please input an URL to github repositoy you want to analyze"
            << std::endl;
  std::cin >> userInput;

  userInputHandler.pickStrategy(userInput);
  userInputHandler.executeStrategy();

  return 0;
}