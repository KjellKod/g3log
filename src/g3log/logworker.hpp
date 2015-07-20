#pragma once
/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================
 * Filename:g3logworker.h  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */
#include "g3log/g3log.hpp"
#include "g3log/sinkwrapper.hpp"
#include "g3log/sinkhandle.hpp"
#include "g3log/filesink.hpp"
#include "g3log/logmessage.hpp"
#include "g3log/std2_make_unique.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>


namespace g3 {
   class LogWorker;
   struct LogWorkerImpl;

   struct DefaultFileLogger {
      DefaultFileLogger(const std::string &log_prefix, const std::string &log_directory);
      std::unique_ptr<LogWorker> worker;
      std::unique_ptr<g3::SinkHandle<g3::FileSink>> sink;

   };

   struct LogWorkerImpl final {
      typedef std::shared_ptr<g3::internal::SinkWrapper> SinkWrapperPtr;
      std::vector<SinkWrapperPtr> _sinks;
      std::unique_ptr<kjellkod::Active> _bg; // do not change declaration order. _bg must be destroyed before sinks

      LogWorkerImpl();
      ~LogWorkerImpl() = default;

      void bgSave(g3::LogMessagePtr msgPtr);
      void bgFatal(FatalMessagePtr msgPtr);

      LogWorkerImpl(const LogWorkerImpl &) = delete;
      LogWorkerImpl &operator=(const LogWorkerImpl &) = delete;
   };

   class LogWorker final {
      LogWorker() = default;
      void addWrappedSink(std::shared_ptr<g3::internal::SinkWrapper> wrapper);

      LogWorkerImpl _impl;
      LogWorker(const LogWorker &) = delete;
      LogWorker &operator=(const LogWorker &) = delete;


   public:
      ~LogWorker();
      static g3::DefaultFileLogger createWithDefaultLogger(const std::string &log_prefix, const std::string &log_directory);
      static std::unique_ptr<LogWorker> createWithNoSink();


      /// pushes in background thread (asynchronously) input messages to log file
      void save(LogMessagePtr entry);

      /// Will push a fatal message on the queue, this is the last message to be processed
      /// this way it's ensured that all existing entries were flushed before 'fatal'
      /// Will abort the application!
      void fatal(FatalMessagePtr fatal_message);

      template<typename T, typename DefaultLogCall>
      std::unique_ptr<g3::SinkHandle<T>> addSink(std::unique_ptr<T> real_sink, DefaultLogCall call) {
         using namespace g3;
         using namespace g3::internal;
         auto sink = std::make_shared<Sink<T>> (std::move(real_sink), call);
         addWrappedSink(sink);
         return std2::make_unique<SinkHandle<T>> (sink);
      }
   };
} // g3
