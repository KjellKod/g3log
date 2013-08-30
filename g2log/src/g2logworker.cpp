/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================
 * Filename:g2LogWorker.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not under copywrite protection. First published at KjellKod.cc
 * ********************************************* */

#include "g2logworker.h"

#include <cassert>
#include <functional>



#include "active.h"
#include "g2log.h"
#include "g2time.hpp"
#include "g2future.h"
#include "crashhandler.hpp"

using namespace g2;
using namespace g2::internal;

struct g2LogWorkerImpl {
  typedef std::shared_ptr<g2::internal::SinkWrapper> SinkWrapperPtr;
  std::unique_ptr<kjellkod::Active> _bg;
  std::vector<SinkWrapperPtr> _sinks;

  g2LogWorkerImpl() : _bg(kjellkod::Active::createActive()) {  }

  ~g2LogWorkerImpl() {
    std::cout << "g2logworkerpimpl in destructor\n"; 
    if(!_bg) { std::cout << "g2sink: fatal failure, no active object in sink\n"; return;}
    _bg.reset(); 
    std::cout << "g2logworker active object destroyed. done sending exit messages to all sinks\n"; 
  }

  void bgSave(g2::internal::LogEntry msg) {
    for (auto& sink : _sinks) {
      sink->send(msg);
    }

    if (_sinks.empty()) {
      std::string err_msg{"g2logworker has no sinks. Message: ["};
      err_msg.append(msg).append({"]\n"});
      std::cerr << err_msg;
    }
  }

  void bgFatal(g2::internal::FatalMessage fatal_message) {
    auto entry = fatal_message.message_;
    bgSave(entry);
    std::string end_message{"Exiting after fatal event. Log flushed sucessfully to disk.\n"};
    bgSave(end_message);
    std::cerr << "g2log sinks are flushed. Now exiting after receiving fatal event\n" << std::flush;

    _sinks.clear(); // flush all queues
    exitWithDefaultSignalHandler(fatal_message.signal_id_);

    // should never reach this point
    perror("g2log exited after receiving FATAL trigger. Flush message status: ");
  }
};



// Default constructor will have one sink: g2filesink.

g2LogWorker::g2LogWorker()
: _pimpl(std2::make_unique<g2LogWorkerImpl>()) {
}

g2LogWorker::~g2LogWorker() { 
  _pimpl->_bg->send([this]{_pimpl->_sinks.clear();}); 
 }

void g2LogWorker::save(g2::internal::LogEntry msg) {
  _pimpl->_bg->send([this, msg] { _pimpl->bgSave(msg); });
}

void g2LogWorker::fatal(g2::internal::FatalMessage fatal_message) {
  _pimpl->_bg->send([this, fatal_message] {_pimpl->bgFatal(fatal_message); });
}

void g2LogWorker::addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> sink) {
  auto bg_addsink_call = [this, sink] { _pimpl->_sinks.push_back(sink); };
  auto token_done = g2::spawn_task(bg_addsink_call, _pimpl->_bg.get());
  token_done.wait();
}


  
  g2LogWorker::DefaultWorkerPair g2LogWorker::createWithDefaultFileSink(const std::string& log_prefix, const std::string& log_directory)
  {
    auto logger = g2LogWorker::createWithNoSink();
    auto handle = logger->addSink(std2::make_unique<g2::g2FileSink>(log_prefix, log_directory), &g2FileSink::fileWrite);
    
    auto pair = std::pair<std::unique_ptr<g2LogWorker>, std::unique_ptr<g2::SinkHandle<g2FileSink>>  >(std::move(logger), std::move(handle));
    return pair;
  }
  
  std::unique_ptr<g2LogWorker> g2LogWorker::createWithNoSink() 
  {
    return std::unique_ptr<g2LogWorker>(new g2LogWorker);
  }