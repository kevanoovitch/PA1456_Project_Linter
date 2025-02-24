#include "scanner.h"
#include "constants.h"
#include "regex"
#include <functional>
#include <unordered_map>
#include <utility>

using namespace constants;

/**********************************************************
 *                          Scanner                       *
 **********************************************************/

Scanner::Scanner() {
  this->mySearcher = new Searcher(this);
  this->myResults = std::make_shared<scanResults>();
  this->repoPath = REPOSITORY_PATH;
  this->fileManagerPtr = new fileManager;
  this->repo = nullptr;
  this->myGitScanner = new GitScanner(this);
}

Scanner::Scanner(const inputHandler &inputHandler) {
  this->mySearcher = new Searcher(this);
  this->myResults = std::make_shared<scanResults>();
  this->repoPath = REPOSITORY_PATH;
  this->fileManagerPtr = new fileManager;
  this->repo = inputHandler.repo;
  this->myGitScanner = new GitScanner(this);
}

Scanner::~Scanner() {
  delete this->mySearcher;
  delete this->fileManagerPtr;
  delete this->myGitScanner;
}

void Scanner::scanForWorkflow() {

  // Workflow scan has to first make sure the requried Dir exists

  if (fileManagerPtr->dirExists(WORKFLOW_PATH) == false) {

    myResults->foundMap[WORKFLOW_STRING] = false;

  } else {

    std::vector<std::string> workflowFileExtensions = {".yml", ".yaml"};

    scanFor(workflowFileExtensions, WORKFLOW_STRING);
  }
}

void Scanner::scanFor(std::vector<std::string> searchAlts,
                      const std::string NAME) {

  std::vector<std::string> resultVectorPaths;
  bool foundAtleastOnce = false;
  for (int i = 0; i < searchAlts.size(); i++) {

    resultVectorPaths = mySearcher->searchFor(REPOSITORY_PATH, searchAlts[i]);

    if (resultVectorPaths.size() == 0 && foundAtleastOnce == false) {
      // no paths meaning found nothing
      this->setFoundMap(false, NAME);
    } else {
      // found one or more readme files
      foundAtleastOnce = true;
      this->setFoundMap(true, NAME);

      for (int i = 0; i < resultVectorPaths.size(); i++) {
        // Push back each path to the result struct
        myResults->pathsMap[NAME].push_back(resultVectorPaths[i]);
      }
    }
  }
}

void Scanner::setFoundMap(bool isFound, std::string Name) {
  if (isFound == true) {
    myResults->foundMap[Name] = true;
  }
}

void Scanner::pushBackPath(std::pair<std::string, std::string> nameAndPath) {

  myResults->pathsMap[nameAndPath.first].push_back(nameAndPath.second);
}

void Scanner::scanGitAttributes() {

  if (this->repo == nullptr) {
    std::cerr << "Error in Scanner::scanGitAttributes() repo was nullptr\n";
    return;
  }

  // count commits
  int nrOfCommits = this->myGitScanner->countCommits(this->repo);

  // build set of contributors
  std::set<std::string> Contributors =
      this->myGitScanner->countContributors(this->repo);

  // Write results to struct

  this->myResults->resultContributors = Contributors;
  this->myResults->resultNrOfCommits = nrOfCommits;
}

/**********************************************************
 *                          Searcher                      *
 **********************************************************/

Searcher::Searcher(Scanner *scanner) { this->myScanner = scanner; }

std::vector<std::string> Searcher::searchFor(std::string path,
                                             std::string searchFor) {
  std::vector<std::string> pathsToFoundFiles;
  std::string pattern("(" + searchFor + "$)");

  std::regex r(pattern);

  for (auto const &dir_entry :
       std::filesystem::recursive_directory_iterator(path)) {
    std::string filenamePathRelative =
        dir_entry.path().string(); // Convert to string

    if (regex_search(filenamePathRelative, r)) {

      std::string filenamePathAbsolute =
          std::filesystem::absolute(dir_entry).string();

      pathsToFoundFiles.push_back(filenamePathAbsolute);
    }
  }

  return pathsToFoundFiles;
};

/**********************************************************
 *                          gitScanner                    *
 **********************************************************/

GitScanner::GitScanner(Scanner *ptr) { this->myScanner = ptr; }

int GitScanner::countCommits(git_repository *repository) {

  int commitCount = 0;
  git_revwalk *walker;

  if (repository == nullptr) {
    std::cerr << "Error in countCommits() repoPtr was nullptr \n";
    return -1;
  }

  int error = git_revwalk_new(&walker, repository);
  error = git_revwalk_push_head(walker);

  git_oid oid;
  while (!git_revwalk_next(&oid, walker)) {
    commitCount++;
  }

  if (error != 0) {

    return -1;
  }

  git_revwalk_free(walker);
  return commitCount;
}

std::set<std::string>
GitScanner::countContributors(git_repository *repository) {
  std::set<std::string> result;

  int commitCount = 0;
  git_revwalk *walker;

  if (repository == nullptr) {

    throw std::runtime_error(
        "Error in countContributors() repoPtr was nullptr \n");
  }

  int error = git_revwalk_new(&walker, repository);
  error = git_revwalk_push_head(walker);

  git_oid oid;
  while (!git_revwalk_next(&oid, walker)) {

    git_commit *commit = nullptr;

    git_commit_lookup(&commit, repository, &oid);

    const git_signature *author = git_commit_author(commit);
    if (author) {
      std::string contributor =
          std::string(author->name) + " <" + author->email + ">";
      result.insert(contributor);
    }
  }

  git_revwalk_free(walker);

  return result;
}
