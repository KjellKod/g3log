/* 
 * File:   test_helper__restore_logger.h
 * Author: kjell
 *
 * Created on July 13, 2013, 4:46 PM
 */

#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <algorithm>
#include "g2logworker.h"

namespace testing_helpers {

   bool removeFile(std::string path_to_file);
   
/** After initializing ScopedCout all std::couts is redirected to the buffer
 @verbatim
 Example: 
  stringstream buffer;   
  ScopedCout guard(&buffer);
  cout << "Hello World";
  ASSERT_STREQ(buffer.str().c_str(), "Hello World"); */
class ScopedOut {
  std::ostream& _out_type;
  std::streambuf* _old_cout;
public:
  explicit ScopedOut(std::ostream& out_type, std::stringstream* buffer)
     : _out_type(out_type)
     , _old_cout(_out_type.rdbuf()) {
      _out_type.rdbuf(buffer->rdbuf());
     }
      
  virtual ~ScopedOut() {
      _out_type.rdbuf(_old_cout);
   }
};

 
 
 


/// RAII cluttering files cleanup
class LogFileCleaner 
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



/** RAII temporarily replace of logger
 *  and restoration of original logger at scope end*/
struct RestoreFileLogger {
  explicit RestoreFileLogger(std::string directory);
  ~RestoreFileLogger();
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


struct ScopedLogger {
    ScopedLogger();
    virtual ~ScopedLogger();
    
    g2LogWorker* get();
    g2LogWorker* _previousWorker;
    std::unique_ptr<g2LogWorker> _currentWorker;
};
} // testing_helpers




