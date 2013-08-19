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

#include "g2log.h"
#include "g2sinkwrapper.h"
#include "g2sinkhandle.h"
#include "g2filesink.hpp"
#include "std2_make_unique.hpp"

struct g2LogWorkerImpl;

class g2LogWorker {
  g2LogWorker();    // Create only through factory  
  void addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> wrapper);

  std::unique_ptr<g2LogWorkerImpl> _pimpl;
  g2LogWorker(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;
  g2LogWorker& operator=(const g2LogWorker&); // c++11 feature not yet in vs2010 = delete;



public:
  virtual ~g2LogWorker();
  typedef std::pair<std::unique_ptr<g2LogWorker>, std::unique_ptr<g2::SinkHandle<g2::g2FileSink>> >  DefaultWorkerPair;
  static DefaultWorkerPair  createWithDefaultFileSink(const std::string& log_prefix, const std::string& log_directory); 
  static std::unique_ptr<g2LogWorker> createWithNoSink();

  
  /// pushes in background thread (asynchronously) input messages to log file
  void save(g2::internal::LogEntry entry);

  /// Will push a fatal message on the queue, this is the last message to be processed
  /// this way it's ensured that all existing entries were flushed before 'fatal'
  /// Will abort the application!
  void fatal(g2::internal::FatalMessage fatal_message);

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
//problemet är att det verkar inte som om min sink avslutas. är det p.g.a. shared_ptr_
//printout från g2FileSink borde synas
//Tips. Gör unit test massvis med loggers som tar in bolean ref som sätts i destruktorn        
//
//1. fixa filechange testet först
//2. sen jämför mitt andra projekt och wrapper.h .     
//        speciellt main, addSink och de underliggande funktionerna
//        
//3) ha worker i wrapper som en unik inte shared klass.
//        
//        När allt är klart: 
//        a) överväg att istället för pair ha en strukt med namnet
//        defaultlogger med samma API som dagens logger. 
//    
//b) andra logger creation skapar inte en DefaultLogger utan en 
//Logger
//
//4) n när allt fungerar bör namnen ändras. 
//filnamnen borde heta g2...
//klassnamnen borde heta g2::whatever  inte g2::g2Whate
#endif // LOG_WORKER_H_
