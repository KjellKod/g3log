
#include <gtest/gtest.h>
#include "testing_helpers.h"
#include "g2log.h"
#include "g2logworker.h"
#include "std2_make_unique.hpp"

using namespace std;
namespace {
  g2LogWorker* oldworker = nullptr;
}

ScopedCout::ScopedCout(std::stringstream* buffer)
: _old_cout(std::cout.rdbuf()) {
  cout.rdbuf(buffer->rdbuf());
}

ScopedCout::~ScopedCout() {
  cout.rdbuf(_old_cout);
}

RestoreLogger::RestoreLogger(std::string directory)
: logger_(std2::make_unique<g2LogWorker>("UNIT_TEST_LOGGER", directory)) {

  oldworker = g2::shutDownLogging();
  g2::initializeLogging(logger_.get());
  g2::internal::changeFatalInitHandlerForUnitTesting();

  std::future<std::string> filename(logger_->logFileName());
  if (!filename.valid()) ADD_FAILURE();
  log_file_ = filename.get();
}

RestoreLogger::~RestoreLogger() {
  reset();
  g2::shutDownLogging();
  if (nullptr != oldworker) g2::initializeLogging(oldworker);
  if (0 != remove(log_file_.c_str()))
    ADD_FAILURE();
}

void RestoreLogger::reset() {
  logger_.reset();
}





namespace testing_helper__cleaner {

  bool removeFile(std::string path_to_file) {
    return (0 == std::remove(path_to_file.c_str()));
  }
}

size_t LogFileCleaner::size() {
  return logs_to_clean_.size();
}

LogFileCleaner::~LogFileCleaner() {
  std::lock_guard<std::mutex> lock(g_mutex);
  {
    for (std::string p : logs_to_clean_) {
      if (false == testing_helper__cleaner::removeFile(p)) {
        ADD_FAILURE() << "UNABLE to remove: " << p.c_str() << std::endl;
      }
    }
    logs_to_clean_.clear();
  } // mutex
}

void LogFileCleaner::addLogToClean(std::string path_to_log) {
  std::lock_guard<std::mutex> lock(g_mutex);
  {
    if (std::find(logs_to_clean_.begin(), logs_to_clean_.end(), path_to_log.c_str()) == logs_to_clean_.end())
      logs_to_clean_.push_back(path_to_log);
  }
}
