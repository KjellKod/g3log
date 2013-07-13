
#include <gtest/gtest.h>
#include "testing_helpers.h"
#include "g2log.h"
#include "g2logworker.h"

using namespace std;

ScopedCout::ScopedCout(std::stringstream* buffer) 
: _old_cout(std::cout.rdbuf()) {
  cout.rdbuf(buffer->rdbuf());
}

ScopedCout::~ScopedCout() {  cout.rdbuf(_old_cout);}

RestoreLogger::RestoreLogger(std::string directory)
: logger_(new g2LogWorker("UNIT_TEST_LOGGER", directory)) {
  g2::initializeLogging(logger_.get());
  g2::internal::changeFatalInitHandlerForUnitTesting();

  std::future<std::string> filename(logger_->logFileName());
  if (!filename.valid()) ADD_FAILURE();
  log_file_ = filename.get();
}

RestoreLogger::~RestoreLogger() {
  reset();
  g2::shutDownLogging();
  if (0 != remove(log_file_.c_str()))
    ADD_FAILURE();
}

void RestoreLogger::reset() {
  logger_.reset();
}