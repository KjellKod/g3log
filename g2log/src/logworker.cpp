/* *************************************************
 * Filename:logworker.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include "logworker.h"
#include <iostream>
#include <string>
#include <functional>

#include "privatelogworker.h"

 LogWorker::LogWorker(const std::string& log_prefix, const std::string& log_directory)
   :  background_worker_(new PrivateLogWorker(log_prefix, log_directory))
   , log_file_with_path_(background_worker_->log_file_with_path_)
 {
 }

 LogWorker::~LogWorker()
 {
   background_worker_.reset();
   //std::cout << "\nLogWorker finished with log: " << log_file_with_path_ << std::endl << std::flush;
 }

 void LogWorker::save(g2::internal::LogEntry msg)
 {
   background_worker_->bg_->send(std::tr1::bind(&PrivateLogWorker::backgroundFileWrite, background_worker_.get(), msg));
 }


 std::string LogWorker::logFileName() const
 {
   return log_file_with_path_;
 }

