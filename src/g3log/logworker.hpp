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
 * PUBLIC DOMAIN and Not copyrighted. First published at KjellKod.cc
 * ********************************************* */
#include "g3log/g3log.hpp"
#include "g3log/sinkwrapper.hpp"
#include "g3log/sinkhandle.hpp"
#include "g3log/filesink.hpp"
#include "g3log/logmessage.hpp"
#include <memory>

#include <memory>
#include <string>
#include <vector>


namespace g3 {
   class LogWorker;
   struct LogWorkerImpl;
   using FileSinkHandle = g3::SinkHandle<g3::FileSink>;

   /// Background side of the LogWorker. Internal use only
   struct LogWorkerImpl final {
      typedef std::shared_ptr<g3::internal::SinkWrapper> SinkWrapperPtr;
      std::vector<SinkWrapperPtr> _sinks;
      std::unique_ptr<kjellkod::Active> _bg; // do not change declaration order. _bg must be destroyed before sinks

      LogWorkerImpl();
      ~LogWorkerImpl() = default;

      void bgSave(g3::LogMessagePtr msgPtr);
      void bgFatal(FatalMessagePtr msgPtr);

      LogWorkerImpl(const LogWorkerImpl&) = delete;
      LogWorkerImpl& operator=(const LogWorkerImpl&) = delete;
   };


   /// Front end of the LogWorker.  API that is useful is
   /// addSink( sink, default_call ) which returns a handle to the sink. See below and README for usage example
   /// save( msg ) : internal use
   /// fatal ( fatal_msg ) : internal use
   class LogWorker final {
      LogWorker() = default;
      void addWrappedSink(std::shared_ptr<g3::internal::SinkWrapper> wrapper);

      LogWorkerImpl _impl;
      LogWorker(const LogWorker&) = delete;
      LogWorker& operator=(const LogWorker&) = delete;


    public:
      ~LogWorker();

      /// Creates the LogWorker with no sinks. See example below on @ref addSink for how to use it
      /// if you want to use the default file logger then see below for @ref addDefaultLogger
      static std::unique_ptr<LogWorker> createLogWorker();


      /**
      A convenience function to add the default g3::FileSink to the log worker
       @param log_prefix that you want
       @param log_directory where the log is to be stored.
       @return a handle for API access to the sink. See the README for example usage

       @verbatim
       Example:
       using namespace g3;
       std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
       auto handle = addDefaultLogger("my_test_log", "/tmp");
       initializeLogging(logworker.get()); // ref. g3log.hpp

       std::future<std::string> log_file_name = sinkHandle->call(&FileSink::fileName);
       std::cout << "The filename is: " << log_file_name.get() << std::endl;
       //   something like: /tmp/my_test_log.g3log.20150819-100300.log
       */
      std::unique_ptr<FileSinkHandle> addDefaultLogger(const std::string& log_prefix, const std::string& log_directory, const std::string& default_id = "g3log");

      /// Adds a sink and returns the handle for access to the sink
      /// @param real_sink unique_ptr ownership is passed to the log worker
      /// @param call the default call that should receive either a std::string or a LogMessageMover message
      /// @return handle to the sink for API access. See usage example below at @ref addDefaultLogger
      template<typename T, typename DefaultLogCall>
      std::unique_ptr<g3::SinkHandle<T>> addSink(std::unique_ptr<T> real_sink, DefaultLogCall call) {
         using namespace g3;
         using namespace g3::internal;
         auto sink = std::make_shared<Sink<T>> (std::move(real_sink), call);
         addWrappedSink(sink);
         return std::make_unique<SinkHandle<T>> (sink);
      }


      /// Removes a sink. This is a synchronous call.
      /// You are guaranteed that the sink is removed by the time the call returns
      /// @param sink_handle the ownership of the sink handle is given
      template<typename T>
      void removeSink(std::unique_ptr<SinkHandle<T>> sink_handle) {
         if (sink_handle) {
            // sink_handle->sink().use_count() is 1 at this point
            // i.e. this would be safe as long as no other weak_ptr to shared_ptr conversion
            // was made by the client: assert(sink_handle->sink().use_count()  == 0);
            auto weak_ptr_sink = sink_handle->sink(); {
               auto bg_removesink_call = [this, weak_ptr_sink] {
                  auto shared_sink = weak_ptr_sink.lock();
                  if (shared_sink) {
                     _impl._sinks.erase(std::remove(_impl._sinks.begin(), _impl._sinks.end(), shared_sink), _impl._sinks.end());
                  }
               };
               auto token_done = g3::spawn_task(bg_removesink_call, _impl._bg.get());
               token_done.wait();
            }
            // sink_handle->sink().use_count() is 1 at this point.
            // i.e. this would be safe: assert(sink_handle->sink().use_count()  == 0);
            // as long as the client has not converted more instances from the weak_ptr
         }
      }

      /// This will clear/remove all the sinks. If a sink shared_ptr was retrieved via the sink
      /// handle then the sink will be removed internally but will live on in the client's instance
      void removeAllSinks() {
         auto bg_clear_sink_call = [this] { _impl._sinks.clear(); };
         auto token_cleared = g3::spawn_task(bg_clear_sink_call, _impl._bg.get());
         token_cleared.wait();
      }



      /// internal:
      /// pushes in background thread (asynchronously) input messages to log file
      void save(LogMessagePtr entry);

      /// internal:
      //  pushes a fatal message on the queue, this is the last message to be processed
      /// this way it's ensured that all existing entries were flushed before 'fatal'
      /// Will abort the application!
      void fatal(FatalMessagePtr fatal_message);


   };
} // g3
