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
#include <cassert>
#include <mutex>
#include <csignal>

#include "g2logworker.hpp"
#include "crashhandler.hpp"
#include "g2loglevels.hpp"
#include "g2time.hpp"

namespace g2
{
  namespace internal
  {
    std::once_flag g_start_time_flag;
    g2::steady_time_point g_start_time;
    std::once_flag g_initialize_flag;
    g2LogWorker* g_logger_instance = nullptr;  // instantiated and OWNED somewhere else (main)
    std::mutex g_logging_init_mutex;

    bool isLoggingInitialized(){
       return g_logger_instance != nullptr; 
    }

    long microsecondsCounter()
    {
      std::call_once(g_start_time_flag, [](){g_start_time =  std::chrono::steady_clock::now();});
      steady_time_point  now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::microseconds>(g_start_time - now).count();
    }

    void saveMessage(const LogEntry& log_entry) {g_logger_instance->save(log_entry);}
  } // internal


  // signalhandler and internal clock is only needed to install once
  // for unit testing purposes the initializeLogging might be called
  // several times... for all other practical use, it shouldn't!
  void initializeLogging(g2LogWorker *bgworker)
  {
    std::call_once(internal::g_initialize_flag, [](){installSignalHandler();});
    std::lock_guard<std::mutex> lock(internal::g_logging_init_mutex);
#ifdef G2_DYNAMIC_LOGGING
    setLogLevel(DEBUG, true);
    setLogLevel(INFO, true);
    setLogLevel(WARNING, true);
    setLogLevel(FATAL, true);
#endif
    CHECK(!internal::isLoggingInitialized());
    CHECK(bgworker != nullptr);
    internal::g_logger_instance = bgworker;
}


  g2LogWorker* shutDownLogging()
  {
    std::lock_guard<std::mutex> lock(internal::g_logging_init_mutex);
    g2LogWorker *backup = internal::g_logger_instance;
    internal::g_logger_instance = nullptr;
    return backup;
  }


  namespace internal
  {
    void fatalCallToLogger(FatalMessage message)
    {
      // real fatal call to logger
      internal::g_logger_instance->fatal(message);
    }

    void fatalCallForUnitTest(g2::internal::FatalMessage fatal_message)
    {
      // mock fatal call, not to logger: used by unit test
      assert(internal::g_logger_instance != nullptr);
      internal::g_logger_instance->save(fatal_message.message_); // calling 'save' instead of 'fatal'
      throw std::runtime_error(fatal_message.message_);
    }

    // By default this function pointer goes to \ref fatalCall;
    void (*g_fatal_to_g2logworker_function_ptr)(FatalMessage) = fatalCallToLogger;




    // The default, initial, handling to send a 'fatal' event to g2logworker
    // the caller will stay here, eternally, until the software is aborted
    void fatalCall(FatalMessage message)
    {
      g_fatal_to_g2logworker_function_ptr(message);
      while(true){std::this_thread::sleep_for(std::chrono::seconds(1));}
    }



    // REPLACE fatalCallToLogger for fatalCallForUnitTest
    // This function switches the function pointer so that only
    //      'unitTest' mock-fatal calls are made.
    void changeFatalInitHandlerForUnitTesting()
    {
      g_fatal_to_g2logworker_function_ptr = fatalCallForUnitTest;
    }


    // represents the actual fatal message
    FatalMessage::FatalMessage(std::string message, FatalType type, int signal_id)
      : message_(message)
      , type_(type)
      , signal_id_(signal_id){}



    // used to RAII trigger fatal message sending to g2LogWorker
    FatalTrigger::FatalTrigger(const FatalMessage &message)
      : message_(message){}

    // at destruction, flushes fatal message to g2LogWorker
    FatalTrigger::~FatalTrigger()
    {
      // either we will stay here eternally, or it's in unit-test mode
      // then we throw a std::runtime_error (and never hit sleep)
      fatalCall(message_);
    }


  } // g2::internal
} // g2
