/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================
 *
 * Filename:g2log.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited since it was built on public-domain software and at least in "spirit" influenced
 * from the following sources
 * 1. kjellkod.cc ;)
 * 2. Dr.Dobbs, Petru Marginean:  http://drdobbs.com/article/printableArticle.jhtml?articleId=201804215&dept_url=/cpp/
 * 3. Dr.Dobbs, Michael Schulze: http://drdobbs.com/article/printableArticle.jhtml?articleId=225700666&dept_url=/cpp/
 * 4. Google 'glog': http://google-glog.googlecode.com/svn/trunk/doc/glog.html
 * 5. Various Q&A at StackOverflow
 * ********************************************* */

#include "g2log.hpp"
#include <cstdio>    // vsnprintf
#include <mutex>
#include <csignal>
#include <memory>
#include <iostream>

#include "std2_make_unique.hpp"
#include "g2logworker.hpp"
#include "crashhandler.hpp"
#include "g2logmessage.hpp"

namespace {
   std::once_flag g_initialize_flag;
   g2::LogWorker* g_logger_instance = nullptr; // instantiated and OWNED somewhere else (main)
   std::mutex g_logging_init_mutex;

   std::unique_ptr<g2::LogMessage> g_first_unintialized_msg = {nullptr};
   std::once_flag g_set_first_uninitialized_flag;
   std::once_flag g_save_first_unintialized_flag;

}





namespace g2 {
   // signalhandler and internal clock is only needed to install once
   // for unit testing purposes the initializeLogging might be called
   // several times... 
   //                    for all other practical use, it shouldn't!

   void initializeLogging(LogWorker *bgworker) {
      std::call_once(g_initialize_flag, []() {
         installSignalHandler(); });
      std::lock_guard<std::mutex> lock(g_logging_init_mutex);
      CHECK(!internal::isLoggingInitialized());
      CHECK(bgworker != nullptr);

      // Save the first uninitialized message, if any     
      std::call_once(g_save_first_unintialized_flag, [&bgworker] {
         if (g_first_unintialized_msg) {
            bgworker->save(LogMessagePtr{std::move(g_first_unintialized_msg)});
         }
      });

      g_logger_instance = bgworker;
   }





   namespace internal {

      bool isLoggingInitialized() {
         return g_logger_instance != nullptr;
      }

      /** 
       * Shutdown the logging by making the pointer to the background logger to nullptr. The object is not deleted
       * that is the responsibility of its owner. * 
       */
      void shutDownLogging() {
         std::lock_guard<std::mutex> lock(g_logging_init_mutex);
         g_logger_instance = nullptr;
      }

      /** Same as the Shutdown above but called by the destructor of the LogWorker, thus ensuring that no further
       *  LOG(...) calls can happen to  a non-existing LogWorker. 
       *  @param active MUST BE the LogWorker initialized for logging. If it is not then this call is just ignored
       *         and the logging continues to be active.
       * @return true if the correct worker was given,. and shutDownLogging was called 
       */
      bool shutDownLoggingForActiveOnly(LogWorker* active) {
         if (isLoggingInitialized() && nullptr != active && (active != g_logger_instance)) {
            LOG(WARNING) << "\n\t\tAttempted to shut down logging, but the ID of the Logger is not the one that is active."
                    << "\n\t\tHaving multiple instances of the g2::LogWorker is likely a BUG"
                    << "\n\t\tEither way, this call to shutDownLogging was ignored"
                    << "\n\t\tTry g2::internal::shutDownLogging() instead";
            return false;
         }
         shutDownLogging();
         return true;
      }



      // explicits copy of all input. This is makes it possibly to use g3log across dynamically loaded libraries
      // i.e. (dlopen + dlsym) 

      void saveMessage(const char* entry, const char* file, int line, const char* function, const LEVELS& level,
              const char* boolean_expression, int fatal_signal, const char* stack_trace) {
         LEVELS msgLevel{level};
         LogMessagePtr message{std2::make_unique<LogMessage>(file, line, function, msgLevel)};
         message.get()->write().append(entry);
         message.get()->setExpression(boolean_expression);

         if (internal::wasFatal(level)) {
            message.get()->write().append(stack_trace);
            FatalMessagePtr fatal_message{std2::make_unique<FatalMessage>(*(message._move_only.get()), fatal_signal)};
            // At destruction, flushes fatal message to g2LogWorker
            // either we will stay here until the background worker has received the fatal
            // message, flushed the crash message to the sinks and exits with the same fatal signal
            //..... OR it's in unit-test mode then we throw a std::runtime_error (and never hit sleep)
            fatalCall(fatal_message);
         } else {
            pushMessageToLogger(message);
         }
      }

      /**
       * save the message to the logger. In case of called before the logger is instantiated
       * the first message will be saved. Any following subsequent unitnialized log calls 
       * will be ignored. 
       * 
       * The first initialized log entry will also save the first uninitialized log message, if any
       * @param log_entry to save to logger
       */
      void pushMessageToLogger(LogMessagePtr incoming) { // todo rename to Push SavedMessage To Worker
         // Uninitialized messages are ignored but does not CHECK/crash the logger  
         if (!internal::isLoggingInitialized()) {
            std::call_once(g_set_first_uninitialized_flag, [&] {
               g_first_unintialized_msg = incoming.release();
               std::string err = {"LOGGER NOT INITIALIZED:\n\t\t"};
               err.append(g_first_unintialized_msg->message());
                       std::string& str = g_first_unintialized_msg->write();
                       str.clear();
                       str.append(err); // replace content
                       std::cerr << str << std::endl;
            });
            return;
         }
         
         // logger is initialized
         g_logger_instance->save(incoming);
      }

      /** Fatal call saved to logger. This will trigger SIGABRT or other fatal signal 
       * to exit the program. After saving the fatal message the calling thread
       * will sleep forever (i.e. until the background thread catches up, saves the fatal
       * message and kills the software with the fatal signal.
       */
      void fatalCallToLogger(FatalMessagePtr message) {
         if (!isLoggingInitialized()) {
            std::ostringstream error;
            error << "FATAL CALL but logger is NOT initialized\n"
                    << "SIGNAL: " << message.get()->signal()
                    << "\nMessage: \n" << message.get()->toString() << std::flush;
            std::cerr << error.str() << std::flush;
            internal::exitWithDefaultSignalHandler(message.get()->_signal_id);
         }
         g_logger_instance->fatal(message);
         while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
         }
      }


      // By default this function pointer goes to \ref fatalCallToLogger;
      std::function<void(FatalMessagePtr) > g_fatal_to_g2logworker_function_ptr = fatalCallToLogger;

      /** The default, initial, handling to send a 'fatal' event to g2logworker
       *  the caller will stay here, eternally, until the software is aborted
       * ... in the case of unit testing it is the given "Mock" fatalCall that will
       * define the behaviour.
       */
      void fatalCall(FatalMessagePtr message) {
         g_fatal_to_g2logworker_function_ptr(FatalMessagePtr{std::move(message)});
      }

      /** REPLACE fatalCallToLogger for fatalCallForUnitTest
       * This function switches the function pointer so that only
       * 'unitTest' mock-fatal calls are made.
       * */
      void changeFatalInitHandlerForUnitTesting(std::function<void(FatalMessagePtr) > fatal_call) {
         g_fatal_to_g2logworker_function_ptr = fatal_call;
      }
   } // internal
} // g2



