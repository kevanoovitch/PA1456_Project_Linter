#pragma once
#include "scanner.h"

class resultInterpreter {
public:
  resultInterpreter(scanResults *res);
  bool isFound(std::string entry);
  void printResults();

private:
  scanResults *results;
};