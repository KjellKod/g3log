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

#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <string>
#include <chrono>
#include <future>
#include <functional>


#include "active.h"
#include "g2log.h"
#include "crashhandler.h"
#include "g2time.h"
#include "g2future.h"
#include "g2filesink.h"
#include "std2_make_unique.hpp"

using namespace g2;
using namespace g2::internal;

struct g2LogWorkerImpl
{
 typedef std::shared_ptr<g2::internal::SinkWrapper> SinkWrapperPtr;
 
  g2LogWorkerImpl(){}
  ~g2LogWorkerImpl(){ _bg.reset(); _sinks.clear(); }
  
  std::unique_ptr<kjellkod::Active> _bg;
  std::vector<SinkWrapperPtr> _sinks;
  std::shared_ptr<g2::SinkHandle<g2FileSink>>  _default_sink_handle;
  void bgSave(g2::internal::LogEntry msg) {
    for(auto& sink : _sinks) {
      sink->send(msg);
    }
  }
  
  void bgFatal(g2::internal::FatalMessage fatal_message){
    auto entry = fatal_message.message_;
    bgSave(entry);
    _sinks.clear(); // flush all queues
    exitWithDefaultSignalHandler(fatal_message.signal_id_);
  }
};


// Default constructor will have one sink: g2filesink.
g2LogWorker::g2LogWorker(const std::string& log_prefix, const std::string& log_directory)
  :  _pimpl(std2::make_unique<g2LogWorkerImpl>())
  {
//  auto unique = std::unique_ptr<g2FileSink>(new g2FileSink(log_prefix, log_directory));
//  auto handle = addSink<g2FileSink>(std::move(unique), &g2FileSink::writeToFile);
//  _pimpl._file_handle.reset(handle.release());
  }

g2LogWorker::~g2LogWorker()
{
  _pimpl.reset();
}

void g2LogWorker::save(g2::internal::LogEntry msg)
{
  _pimpl->_bg->send([this, msg]{ _pimpl->bgSave(msg); }); 
  //std::bind(&g2LogWorkerImpl::backgroundFileWrite, _pimpl.get(), msg));
}

void g2LogWorker::fatal(g2::internal::FatalMessage fatal_message)
{
  _pimpl->_bg->send([this, fatal_message]{ _pimpl->bgFatal(fatal_message); });
  //std::bind(&g2LogWorkerImpl::backgroundExitFatal, _pimpl.get(), fatal_message));
}


void g2LogWorker::addWrappedSink(std::shared_ptr<g2::internal::SinkWrapper> sink) {
    auto bg_addsink_call = [this, sink] { _pimpl->_sinks.push_back(sink); };
    auto token_done = g2::spawn_task(bg_addsink_call, _pimpl->_bg.get());
    token_done.wait();
}



  std::shared_ptr<g2::SinkHandle<g2FileSink>>  g2LogWorker::getFileSinkHandle()
  {
     return _pimpl->_default_sink_handle;
  }