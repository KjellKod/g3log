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

#include "g2logworker.hpp"

#include <cassert>
#include <functional>
#include "active.hpp"
#include "g2log.hpp"
#include "g2time.hpp"
#include "g2future.h"
#include "crashhandler.hpp"

#include <iostream> // remove
using namespace g2;
using namespace g2::internal;

struct g2LogWorkerImpl {
  typedef std::shared_ptr<g2::internal::SinkWrapper> SinkWrapperPtr;
  std::unique_ptr<kjellkod::Active> _bg;
  std::vector<SinkWrapperPtr> _sinks;

  g2LogWorkerImpl() : _bg(kjellkod::Active::createActive()) {  }

  ~g2LogWorkerImpl() {
    _bg.reset(); 
  }

  void bgSave(const g2::LogMessage& msg) {
    for (auto& sink : _sinks) {
      sink->send(msg);
    }

    if (_sinks.empty()) {
      std::string err_msg{"g2logworker has no sinks. Message: ["};
      err_msg.append(msg.toString()).append({"]\n"});
      std::cerr << err_msg;
    }
  }

  void bgFatal(const FatalMessage& msg) {
    auto fatal_message = msg;
    fatal_message.stream() <<  "\nExiting after fatal event. Log flushed sucessfully to disk.\n";
    bgSave(fatal_message.copyToLogMessage());

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
// todo move operator
void g2LogWorker::save(const LogMessage& msg) {
  _pimpl->_bg->send([this, msg] { _pimpl->bgSave(msg); }); // TODO std::move
}

void g2LogWorker::fatal(const FatalMessage& fatal_message) {
  _pimpl->_bg->send([this, fatal_message] {_pimpl->bgFatal(fatal_message); });
}

void g2LogWorker::addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> sink) {
  auto bg_addsink_call = [this, sink] { _pimpl->_sinks.push_back(sink); };
  auto token_done = g2::spawn_task(bg_addsink_call, _pimpl->_bg.get());
  token_done.wait();
}


  // Gör en egen super simpel klass/struct med 
  // DefaultFilLogger den ska INTE vara i g2logworker.cpp
  g2::DefaultFileLogger g2LogWorker::createWithDefaultLogger(const std::string& log_prefix, const std::string& log_directory)
  {
    return g2::DefaultFileLogger(log_prefix, log_directory);
  }
  
  std::unique_ptr<g2LogWorker> g2LogWorker::createWithNoSink() 
  {
    return std::unique_ptr<g2LogWorker>(new g2LogWorker);
  }
  
  

DefaultFileLogger::DefaultFileLogger(const std::string& log_prefix, const std::string& log_directory)
: worker(g2LogWorker::createWithNoSink())
, sink(worker->addSink(std2::make_unique<g2::FileSink>(log_prefix, log_directory), &FileSink::fileWrite)) {
}
