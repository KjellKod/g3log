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
#include "g2log.ipp"
#include <cstdio>    // vsnprintf
#include <mutex>
#include <csignal>

#include "g2logworker.hpp"
#include "crashhandler.hpp"
#include "g2loglevels.hpp"


namespace g2 {
   namespace internal {
      std::once_flag g_initialize_flag;
      std::mutex g_logging_init_mutex;

      // ref: g2log.ipp
      bool isLoggingInitialized() {
         return g_logger_instance != nullptr;
      }

      /// Used for test purposes. Should rarely, if ever, be used in production code
      g2LogWorker* shutDownLogging() {
         std::lock_guard<std::mutex> lock(internal::g_logging_init_mutex);
         g2LogWorker *backup = internal::g_logger_instance;
         internal::g_logger_instance = nullptr;
         return backup;
      }
   } // g2::internal


   void initializeLogging(g2LogWorker *bgworker) {
      std::call_once(internal::g_initialize_flag, []() {
         installSignalHandler(); });
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
  
} // g2
