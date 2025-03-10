
#pragma once

#include <string>
struct errorStatus {
  static int statusInt; // 0 = no error, 1 = failed linting 2 = system error
  static std::string statusMessage;
};
