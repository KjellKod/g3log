#ifndef CRASH_HANDLER_H_
#define CRASH_HANDLER_H_
/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/
#include <string>
#include <csignal>

// kjell. Separera på crashhandler.hpp och crashhanlder_internal.hpp
// implementationsfilen kan vara den samma

namespace g2 {
namespace internal {
/** \return signal_name. Ref: signum.hpp and \ref installSignalHandler */
std::string signalName(int signal_number);

/** return calling thread's stackdump*/
std::string stackdump();

/** Re-"throw" a fatal signal, previously caught. This will exit the application
 * This is an internal only function. Do not use it elsewhere. It is triggered
 * from g2log, g2LogWorker after flushing messages to file */
void exitWithDefaultSignalHandler(int signal_number);
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
void installSignalHandler();
}

#endif // CRASH_HANDLER_H_
