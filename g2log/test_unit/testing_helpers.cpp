
#include <gtest/gtest.h>
#include <iostream>
#include "testing_helpers.h"
#include "g2log.hpp"
#include "g2logworker.hpp"
#include "g2filesink.hpp"
#include "std2_make_unique.hpp"

using namespace std;
namespace testing_helpers {

   bool removeFile(std::string path_to_file) {
      return (0 == std::remove(path_to_file.c_str()));
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

   
   
   ScopedLogger::ScopedLogger()
  : _previousWorker(g2::internal::shutDownLogging())
   , _currentWorker(g2LogWorker::createWithNoSink()) {
     g2::initializeLogging(_currentWorker.get());
  }

  ScopedLogger::~ScopedLogger() {
     auto* current = g2::internal::shutDownLogging();  
     CHECK(current == _currentWorker.get());
     if (nullptr != _previousWorker) { 
        g2::initializeLogging(_previousWorker); 
     }
  }
  
   g2LogWorker*  ScopedLogger::get() {
      return _currentWorker.get(); 
   }
   
   
   RestoreFileLogger::RestoreFileLogger(std::string directory)
   : scope_(new ScopedLogger)
   {
      using namespace g2;
      auto filehandler = scope_->get()->addSink(std2::make_unique<FileSink>("UNIT_TEST_LOGGER", directory), &FileSink::fileWrite);

      internal::changeFatalInitHandlerForUnitTesting();
      LOG(INFO) << "Restore logger test ";
      auto filename = filehandler->call(&FileSink::fileName);
      if (!filename.valid()) ADD_FAILURE();
      log_file_ = filename.get();
   }

   RestoreFileLogger::~RestoreFileLogger() {
      scope_.reset();
      if (!removeFile(log_file_))
         ADD_FAILURE();
   }







} // testing_helpers
