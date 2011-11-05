/* *************************************************
 * Filename: privatelogworker.h  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */


#ifndef PRIVATE_LOG_WORKER_H_
#define PRIVATE_LOG_WORKER_H_

#include <string>
#include <memory>
#include <fstream>
#include "active.h"
#include "g2log.h"



/** The actual background worker, while LogWorker gives the
  * asynchronous API to put job in the background the PrivateLogWorker
  * does the actual background thread work */
struct PrivateLogWorker
{
  PrivateLogWorker(const std::string& log_prefix, const std::string& log_directory);
  ~PrivateLogWorker();

  void backgroundFileWrite(g2::internal::LogEntry message);

  std::string log_file_with_path_;
  std::unique_ptr<kjellkod::Active> bg_;
  std::ofstream out;
  g2::internal::time_point start_time_;

private:
  PrivateLogWorker& operator=(const PrivateLogWorker&) = delete; // no assignment, no copy
  PrivateLogWorker(const PrivateLogWorker& other) = delete;

};

#endif
