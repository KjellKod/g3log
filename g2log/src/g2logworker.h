#ifndef G2_LOG_WORKER_H_
#define G2_LOG_WORKER_H_
/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2logworker.h  Framework for Logging and Design By Contract
* Created: 2011 by Kjell Hedström
*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */


#include <memory>
#include <future>
#include <string>
#include "std2_make_unique.hpp"

#include "g2log.h"
#include "g2sinkwrapper.h"
#include "g2sinkhandle.h"


struct g2LogWorkerImpl;
struct g2FileSink;

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

  template<typename T, typename DefaultLogCall>
  std::unique_ptr<g2::SinkHandle<T>> addSink(std::unique_ptr<T> real_sink, DefaultLogCall call) 
  {
    using namespace g2;
    using namespace g2::internal;
    auto shared_sink = std::shared_ptr<T>(real_sink.release());
    auto sink = std::make_shared<Sink<T>>(shared_sink, call);
    auto add_result = addWrappedSink(sink);
    return std2::make_unique<SinkHandle<T>>(sink); 
  }
      
      


  /// DEPRECATED - SHOULD BE Called through a sink handler instead
  /// Attempt to change the current log file to another name/location.
  /// returns filename with full path if successful, else empty string
  //std::future<std::string> changeLogFile(const std::string& log_directory);
  /// Probably only needed for unit-testing or specific log management post logging
  /// request to get log name is processed in FIFO order just like any other background job.
  //std::future<std::string> logFileName();
  std::shared_ptr<g2::SinkHandle<g2FileSink>>  getFileSinkHandle();

private:
  void addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> wrapper);
  
  std::unique_ptr<g2LogWorkerImpl> _pimpl;

  g2LogWorker(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
  g2LogWorker& operator=(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
};


#endif // LOG_WORKER_H_
