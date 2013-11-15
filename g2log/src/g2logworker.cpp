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

namespace g2 {
   
struct LogWorkerImpl {
   typedef std::shared_ptr<g2::internal::SinkWrapper> SinkWrapperPtr;
   std::unique_ptr<kjellkod::Active> _bg;
   std::vector<SinkWrapperPtr> _sinks;

   LogWorkerImpl() : _bg(kjellkod::Active::createActive()) {}

   ~LogWorkerImpl() {
      _bg.reset();
   }

   void bgSave(g2::LogMessagePtr msgPtr) {
      std::shared_ptr<LogMessage> msg(msgPtr.get().release());
      if (msg == nullptr) {
         std::cerr << "ERROR NULLPTR" << std::endl;
         return;
         
      }
      for (auto& sink : _sinks) { 
         sink->send(msg);
      }

      if (_sinks.empty()) {
         std::string err_msg{"g2logworker has no sinks. Message: ["};
         err_msg.append(msg->toString()).append({"]\n"});
         std::cerr << err_msg;
      }
   }

   void bgFatal(FatalMessagePtr msgPtr) {
      std::string signal = msgPtr.get()->signal();
      std::shared_ptr<LogMessage> message(msgPtr.release()); // = msgPtr.get()->copyToLogMessage();
      message->stream() << "\nExiting after fatal event  (" << message->level()
              << "). Exiting with signal: " << signal
              << "\nLog content flushed flushed sucessfully to sink\n\n";
      
      std::cerr << message->message() << std::flush;
      for (auto& sink : _sinks) {
         sink->send(message);
      }
      _sinks.clear(); // flush all queues

      internal::exitWithDefaultSignalHandler(msgPtr.get()->_signal_id);
      // should never reach this point
      perror("g2log exited after receiving FATAL trigger. Flush message status: ");
   }
};




// Default constructor will have one sink: g2filesink.
LogWorker::LogWorker()
: _pimpl(std2::make_unique<LogWorkerImpl>()) {
}

LogWorker::~LogWorker() {
   _pimpl->_bg->send([this] {_pimpl->_sinks.clear();});
}
// todo move operator

void LogWorker::save(LogMessagePtr msg) {
   _pimpl->_bg->send([this, msg] {_pimpl->bgSave(msg); }); 
}

void LogWorker::fatal(FatalMessagePtr fatal_message) {
   _pimpl->_bg->send([this, fatal_message] { _pimpl->bgFatal(fatal_message); });
}

void LogWorker::addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> sink) {
   auto bg_addsink_call = [this, sink] { _pimpl->_sinks.push_back(sink); };
   auto token_done = g2::spawn_task(bg_addsink_call, _pimpl->_bg.get());
   token_done.wait();
}


// Gör en egen super simpel klass/struct med 
// DefaultFilLogger den ska INTE vara i g2logworker.cpp

g2::DefaultFileLogger LogWorker::createWithDefaultLogger(const std::string& log_prefix, const std::string& log_directory) {
   return g2::DefaultFileLogger(log_prefix, log_directory);
}

std::unique_ptr<LogWorker> LogWorker::createWithNoSink() {
   return std::unique_ptr<LogWorker>(new LogWorker);
}

DefaultFileLogger::DefaultFileLogger(const std::string& log_prefix, const std::string& log_directory)
: worker(LogWorker::createWithNoSink())
, sink(worker->addSink(std2::make_unique<g2::FileSink>(log_prefix, log_directory), &FileSink::fileWrite)) {
}

} // g2