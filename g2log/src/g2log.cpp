/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
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

#include "g2logworker.hpp"
#include "crashhandler.hpp"
#include "g2loglevels.hpp"
#include "g2logmessageimpl.hpp"

namespace {
   std::once_flag g_initialize_flag;
   g2LogWorker* g_logger_instance = nullptr; // instantiated and OWNED somewhere else (main)
   std::mutex g_logging_init_mutex;

   std::shared_ptr<g2::LogMessageImpl> g_first_unintialized_msg = {nullptr};
   bool g_uninitialized_call = false;
   std::once_flag g_set_first_uninitialized_flag;
   std::once_flag g_save_first_unintialized_flag;

}





namespace g2 {
   // signalhandler and internal clock is only needed to install once
   // for unit testing purposes the initializeLogging might be called
   // several times... for all other practical use, it shouldn't!


   void initializeLogging(g2LogWorker *bgworker) {
      std::call_once(g_initialize_flag, []() {
         installSignalHandler(); });
      std::lock_guard<std::mutex> lock(g_logging_init_mutex);
#ifdef G2_DYNAMIC_LOGGING
      setLogLevel(DEBUG, true);
      setLogLevel(INFO, true);
      setLogLevel(WARNING, true);
      setLogLevel(FATAL, true);
#endif
      CHECK(!internal::isLoggingInitialized());
      CHECK(bgworker != nullptr);
      g_logger_instance = bgworker;
   }





   namespace internal {

      bool isLoggingInitialized() {
         return g_logger_instance != nullptr;
      }

      /** Should be used for unit testing. Used in production code is likely wrong.*/
      g2LogWorker* shutDownLogging() {
         std::lock_guard<std::mutex> lock(g_logging_init_mutex);
         CHECK(isLoggingInitialized()) << "NO Logger is instantiated ... exiting";
         g2LogWorker *backup = g_logger_instance;
         g_logger_instance = nullptr;
         return backup;
      }


      /**
       * save the message to the logger. In case of called before the logger is instantiated
       * the first message will be saved. Any following subsequent unitnialized log calls 
       * will be ignored. 
       * 
       * The first initialized log entry will also save the first uninitialized log message, if any
       * @param log_entry to save to logger
       */
      void saveMessage(const g2::LogMessage& log_entry) {
         // Uninitialized messages are ignored but does not CHECK/crash the logger  
         if (!internal::isLoggingInitialized()) {
            
            std::call_once(g_set_first_uninitialized_flag, [&] {
               g_first_unintialized_msg = log_entry._pimpl;
               g_uninitialized_call = true;
               std::string err = {"LOGGER NOT INITIALIZED:\n\t\t"};
               err.append(log_entry.message());
               auto& stream = log_entry.stream();
               stream.str("");
               stream << err;
               std::cerr << err << std::endl;
            });
            return;
         }

         // logger is initialized
         // Save the first uninitialized message, if any     
         std::call_once(g_save_first_unintialized_flag, [] {
            if (g_uninitialized_call) {
               LogMessage error_msg{g_first_unintialized_msg};
               g_logger_instance->save(error_msg);
               g_first_unintialized_msg.reset();
            }
         });

         g_logger_instance->save(log_entry);
      }


      /** Fatal call saved to logger. This will trigger SIGABRT or other fatal signal 
       * to exit the program. After saving the fatal message the calling thread
       * will sleep forever (i.e. until the background thread catches up, saves the fatal
       * message and kills the software with the fatal signal.
       */
      void fatalCallToLogger(const FatalMessage& message) {
         if (!isLoggingInitialized()) {
            std::ostringstream error;
            error << "FATAL CALL but logger is NOT initialized\n"
                    << "SIGNAL: " << g2::internal::signalName(message.signal_id_)
                    << "\nMessage: \n" << message.toString() << std::flush;
            std::cerr << error << std::flush;
            internal::exitWithDefaultSignalHandler(message.signal_id_);
         }

         g_logger_instance->fatal(message);

         while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
         }
      }


      // By default this function pointer goes to \ref fatalCallToLogger;
      std::function<void(const FatalMessage&) > g_fatal_to_g2logworker_function_ptr = fatalCallToLogger;


      /** The default, initial, handling to send a 'fatal' event to g2logworker
       *  the caller will stay here, eternally, until the software is aborted
       * ... in the case of unit testing it is the given "Mock" fatalCall that will
       * define the behaviour.
       */
      void fatalCall(const FatalMessage& message) {
         g_fatal_to_g2logworker_function_ptr(message);
      }


      /** REPLACE fatalCallToLogger for fatalCallForUnitTest
       * This function switches the function pointer so that only
       * 'unitTest' mock-fatal calls are made.
       * */
      void changeFatalInitHandlerForUnitTesting(std::function<void(const FatalMessage&) > fatal_call) {
         g_fatal_to_g2logworker_function_ptr = fatal_call;
      }
   } // internal
} // g2



