#ifndef LOG_WORKER_H_
#define LOG_WORKER_H_

/* *************************************************
 * Filename:logworker.h  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */
#include <memory>
#include "g2log.h"

class LogWorkerImpl;

/**
* \param log_prefix is the 'name' of the binary, this give the log name 'LOG-'name'-...
* \param log_directory gives the directory to put the log files */
class LogWorker
{
public:
  LogWorker(const std::string& log_prefix, const std::string& log_directory);
  virtual ~LogWorker();

  /// pushes in background thread (asynchronously) input messages to log file
  void save(g2::internal::LogEntry);

  /// basically only needed for unit-testing or specific log management post logging
  std::string logFileName() const;

private:
  std::unique_ptr<LogWorkerImpl> pimpl_;
  const std::string log_file_with_path_;

  LogWorker(const LogWorker&) = delete; // no assignment, no copy
  LogWorker& operator=(const LogWorker&) = delete;
};



#endif // LOG_WORKER_H_
