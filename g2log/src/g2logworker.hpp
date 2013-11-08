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
#include <utility>

#include "g2log.hpp"
#include "g2sinkwrapper.h"
#include "g2sinkhandle.h"
#include "g2filesink.hpp"
#include "std2_make_unique.hpp"

struct g2LogWorkerImpl;


class g2LogWorker;
namespace g2 {
struct DefaultFileLogger {
    DefaultFileLogger(const std::string& log_prefix, const std::string& log_directory);
    std::unique_ptr<g2LogWorker> worker;
    std::unique_ptr<g2::SinkHandle<g2::FileSink>> sink;
  };
}


class g2LogWorker {
  g2LogWorker();    // Create only through factory  
  void addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> wrapper);

  std::unique_ptr<g2LogWorkerImpl> _pimpl;
  g2LogWorker(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
  g2LogWorker& operator=(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;



public:
  virtual ~g2LogWorker();
  
    
  static g2::DefaultFileLogger  createWithDefaultLogger(const std::string& log_prefix, const std::string& log_directory); 
  static std::unique_ptr<g2LogWorker> createWithNoSink();

  
  /// pushes in background thread (asynchronously) input messages to log file
  void save(g2::LogMessage entry);

  /// Will push a fatal message on the queue, this is the last message to be processed
  /// this way it's ensured that all existing entries were flushed before 'fatal'
  /// Will abort the application!
  void fatal(g2::FatalMessage fatal_message);

  template<typename T, typename DefaultLogCall>
  std::unique_ptr<g2::SinkHandle<T >> addSink(std::unique_ptr<T> real_sink, DefaultLogCall call) {
    using namespace g2;
    using namespace g2::internal;
    auto shared_sink = std::shared_ptr<T>(real_sink.release());
    auto sink = std::make_shared < Sink < T >> (shared_sink, call);
    addWrappedSink(sink);
    return std2::make_unique < SinkHandle < T >> (sink);
  }
};



#endif // LOG_WORKER_H_
