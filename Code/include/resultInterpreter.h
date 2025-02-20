#pragma once
#include "scanner.h"

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/
class resultEntry;

/**********************************************************
 *                          Scan Results                  *
 **********************************************************/

struct scanResults {
  std::unordered_map<std::string, bool> foundMap;
  std::unordered_map<std::string, std::vector<std::string>> pathsMap;

  std::string gitIgnoreHandle;
  std::string licenseHandle;
  std::string workflowHandle;
  std::string readmeHandle;

  // Default constructo for struct
  scanResults();
};

/**********************************************************
 *                  Result Interpreter                    *
 **********************************************************/

class resultInterpreter {
public:
  resultInterpreter(std::shared_ptr<scanResults> res);
  bool isFound(std::string entry);
  void printResults();
  void printDetails();
  void interpretResults();
  std::vector<std::unique_ptr<resultEntry>> AllResultEntries;

private:
  std::shared_ptr<scanResults> results;
  std::unique_ptr<resultEntry> pickAndCreateEntry(std::string name);
};

/**********************************************************
 *                  Result Entry Strategy                 *
 **********************************************************/

class resultEntry {

public:
  resultEntry(std::shared_ptr<scanResults> res);
  resultEntry();
  virtual void indicatorDeterminator() = 0;
  virtual void printEntry() = 0;
  std::string entryName;
  std::vector<std::string> paths;
  bool isFound;
  std::string Indication;
  std::string IndicationReason;

protected:
  std::shared_ptr<scanResults> myResults;
  // Shared algorithms

  void VerifyIfFound(std::string name);
  void noMoreThanOne(std::string name);
  void parentPrintEntry();
};

/**********************************************************
 *                        ReadMe                           *
 **********************************************************/
class readmeEntry : public resultEntry {
public:
  readmeEntry(std::shared_ptr<scanResults> res);
  readmeEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
};

/**********************************************************
 *                        License                         *
 **********************************************************/
class licenseEntry : public resultEntry {

public:
  licenseEntry(std::shared_ptr<scanResults> res) : resultEntry(res){};
  licenseEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
};

/**********************************************************
 *                        Workflow                        *
 **********************************************************/
class workflowEntry : public resultEntry {

public:
  workflowEntry(std::shared_ptr<scanResults> res) : resultEntry(res){};
  workflowEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
};

/**********************************************************
 *                        gitIgnore                       *
 **********************************************************/

class gitignoreEntry : public resultEntry {

public:
  gitignoreEntry(std::shared_ptr<scanResults> res) : resultEntry(res){};
  gitignoreEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
};
