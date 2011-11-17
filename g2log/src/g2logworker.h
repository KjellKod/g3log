#ifndef G2_LOG_WORKER_H_
#define G2_LOG_WORKER_H_

/* *************************************************
 * Filename:g2logworker.h  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */
#include <memory>
#include "g2log.h"

struct g2LogWorkerImpl;

/**
* \param log_prefix is the 'name' of the binary, this give the log name 'LOG-'name'-...
* \param log_directory gives the directory to put the log files */
class g2LogWorker
{
public:
  g2LogWorker(const std::string& log_prefix, const std::string& log_directory);
  virtual ~g2LogWorker();

  /// pushes in background thread (asynchronously) input messages to log file
  void save(g2::internal::LogEntry entry);

  /// Will push a fatal message on the queue, this is the last message to be processed
  /// this way it's ensured that all existing entries were flushed before 'fatal'
  /// Will abort the application!
  void fatal(g2::internal::FatalMessage fatal_message);

  /// basically only needed for unit-testing or specific log management post logging
  std::string logFileName() const;

private:
  std::unique_ptr<g2LogWorkerImpl> pimpl_;
  const std::string log_file_with_path_;

  g2LogWorker(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
  g2LogWorker& operator=(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
};



#endif // LOG_WORKER_H_
