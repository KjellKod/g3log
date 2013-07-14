/* 
 * File:   test_helper__restore_logger.h
 * Author: kjell
 *
 * Created on July 13, 2013, 4:46 PM
 */

#ifndef TEST_HELPER__RESTORE_LOGGER_H
#define	TEST_HELPER__RESTORE_LOGGER_H

#include <memory>
#include <string>
#include <iostream>
#include <mutex>
#include <algorithm>
#include "g2logworker.h"

// After initializing ScopedCout all std::couts is redirected to the buffer
// Example: 
//   stringstream buffer;   
//   ScopedCout guard(&buffer);
//   cout << "Hello World";
//   ASSERT_STREQ(buffer.str().c_str(), "Hello World"); 

class ScopedCout {
  std::streambuf* _old_cout;
public:
  explicit ScopedCout(std::stringstream* buffer);
  ~ScopedCout();
};


namespace testing_helper__cleaner {
  bool removeFile(std::string path_to_file);
}

class LogFileCleaner // RAII cluttering files cleanup
{
private:
  std::vector<std::string> logs_to_clean_;
  std::mutex g_mutex;
public:
  size_t size();

  LogFileCleaner() {
  }
  virtual ~LogFileCleaner();
  void addLogToClean(std::string path_to_log);
};



// RAII temporarily replace of logger
// and restoration of original logger at scope end

struct RestoreLogger {
  explicit RestoreLogger(std::string directory);
  ~RestoreLogger();
  void reset();
  
  std::unique_ptr<g2LogWorker> logger_;

  template<typename Call, typename ... Args >
          typename std::result_of<Call(Args...)>::type callToLogger(Call call, Args&&... args) {
    auto func = std::bind(call, logger_.get(), std::forward<Args>(args)...);
    return func();
  }
  

  std::string logFile() { return log_file_;  }
private:
  std::string log_file_;

};



#endif	/* TEST_HELPER__RESTORE_LOGGER_H */

