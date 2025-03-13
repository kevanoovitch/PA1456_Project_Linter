#pragma once
#include "inputHandler.h"
#include "scanner.h"

/**********************************************************
 *                  Forward Declaration                   *
 **********************************************************/
class resultEntry;
struct scanResults;
class inputHandler;

/**********************************************************
 *                  Result Interpreter                    *
 **********************************************************/

class resultInterpreter {
public:
  resultInterpreter(const inputHandler &inputHandler);
  bool isFound(std::string entry);

  void printDetails();
  void printMinimal();
  void interpretResults();
  void interpretLeaks();
  std::vector<std::unique_ptr<resultEntry>> AllResultEntries;

private:
  void printGitAttributes();
  std::shared_ptr<scanResults> sharedResult;
  std::unique_ptr<resultEntry> pickAndCreateEntry(std::string name);
  std::vector<std::pair<std::string, int>> rankContributors();
  static bool compareContributors(const std::pair<std::string, int> &a,
                                  const std::pair<std::string, int> &b);
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
  std::string readMore;

  bool allowMultiple;
  bool requireContent;
  bool required;

protected:
  std::shared_ptr<scanResults> sharedResult;
  // Shared algorithms

  void VerifyIfFound(std::string name);
  void noMoreThanOne(std::string name);
  void checkContents();
  void parentPrintEntry();
  bool crossRefrenceIgnore(std::string path);
  void implicitIndication();
  void updateError();
  void handleCrossRefrence(std::vector<std::string> &paths);
  std::string makeRelToGitRoot(std::string path);

  void applyFileConfigRules();
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
  licenseEntry(std::shared_ptr<scanResults> res);
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

/**********************************************************
 *                        Leaks Entry                     *
 **********************************************************/

class leaksEntry : public resultEntry {

public:
  leaksEntry(std::shared_ptr<scanResults> res) : resultEntry(res){};
  leaksEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
  std::vector<std::pair<std::string, std::set<std::string>>> IndicationReasons;
};

/**********************************************************
 *                        Tests Entry                     *
 **********************************************************/

class testEntry : public resultEntry {

public:
  testEntry(std::shared_ptr<scanResults> res) : resultEntry(res){};
  testEntry();

private:
  void indicatorDeterminator() override;
  void printEntry() override;
};