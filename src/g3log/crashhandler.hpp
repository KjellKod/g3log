#pragma once

/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/
#include <string>
#include <csignal>
#include "g3log/loglevels.hpp"

// kjell. Separera på crashhandler.hpp och crashhanlder_internal.hpp
// implementationsfilen kan vara den samma
namespace g2 {


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   typedef unsigned long SignalType;
   ///  SIGFPE, SIGILL, and SIGSEGV handling must be installed per thread
   /// on Windows. This is automatically done if you do at least one LOG(...) call
   /// you can also use this function call, per thread so make sure these three
   /// fatal signals are covered in your thread (even if you don't do a LOG(...) call
   void installSignalHandlerForThread();
#else
   typedef int SignalType;
#endif


   namespace internal {
      /** return whether or any fatal handling is still ongoing
       *  this is used by g2log::fatalCallToLogger
       *  only in the case of Windows exceptions (not fatal signals)
       *  are we interested in changing this from false to true to
       *  help any other exceptions handler work with 'EXCEPTION_CONTINUE_SEARCH'*/
      bool blockForFatalHandling();

      /** \return signal_name Ref: signum.hpp and \ref installSignalHandler
      *  or for Windows exception name */
      std::string exitReasonName(const LEVELS &level, g2::SignalType signal_number);

      /** return calling thread's stackdump*/
      std::string stackdump(const char *dump = nullptr);

      /** Re-"throw" a fatal signal, previously caught. This will exit the application
       * This is an internal only function. Do not use it elsewhere. It is triggered
       * from g2log, g2LogWorker after flushing messages to file */
      void exitWithDefaultSignalHandler(const LEVELS &level, g2::SignalType signal_number);
   } // end g2::internal


   // PUBLIC API:
   /** Install signal handler that catches FATAL C-runtime or OS signals
     See the wikipedia site for details http://en.wikipedia.org/wiki/SIGFPE
     See the this site for example usage: http://www.tutorialspoint.com/cplusplus/cpp_signal_handling.hpptm
     SIGABRT  ABORT (ANSI), abnormal termination
     SIGFPE   Floating point exception (ANSI)
     SIGILL   ILlegal instruction (ANSI)
     SIGSEGV  Segmentation violation i.e. illegal memory reference
     SIGTERM  TERMINATION (ANSI)  */
   void installCrashHandler();
}
