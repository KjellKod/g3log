
#include <gtest/gtest.h>
#include <iostream>
#include "testing_helpers.h"
#include "g2log.h"
#include "g2logworker.h"
#include "g2filesink.hpp"
#include "std2_make_unique.hpp"

using namespace std;
namespace {
   g2LogWorker* oldworker = nullptr;


}

namespace testing_helpers {

   bool removeFile(std::string path_to_file) {
      return (0 == std::remove(path_to_file.c_str()));
   }


   RestoreLogger::RestoreLogger(std::string directory)
   : logger_(g2LogWorker::createWithNoSink()) {
      using namespace g2;
      auto filehandler = logger_->addSink(std2::make_unique<g2FileSink>("UNIT_TEST_LOGGER", directory), &g2FileSink::fileWrite);

      oldworker = g2::shutDownLogging();
      initializeLogging(logger_.get());
      internal::changeFatalInitHandlerForUnitTesting();
      LOG(INFO) << "Restore logger test ";
      auto filename = filehandler->call(&g2FileSink::fileName);
      if (!filename.valid()) ADD_FAILURE();
      log_file_ = filename.get();
   }

   RestoreLogger::~RestoreLogger() {
      reset();
      g2::shutDownLogging();
      if (nullptr != oldworker) g2::initializeLogging(oldworker);
      if (!removeFile(log_file_))
         ADD_FAILURE();
   }

   void RestoreLogger::reset() {
      logger_.reset();
   }

   size_t LogFileCleaner::size() {
      return logs_to_clean_.size();
   }

   LogFileCleaner::~LogFileCleaner() {
      std::lock_guard<std::mutex> lock(g_mutex);
      {
         for (const auto& file : logs_to_clean_) {
            if (!removeFile(file)) {
               ADD_FAILURE() << "UNABLE to remove: " << file << std::endl;
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

} // testing_helpers
