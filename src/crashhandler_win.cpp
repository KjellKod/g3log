/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "crashhandler.hpp"
#include "stacktrace_windows.hpp"
#include "g2logmessage.hpp"
#include "g2logmessagecapture.hpp"

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <sstream>
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#error "crashhandler_win.cpp used but not on a windows system"
#endif

#include <process.h> // getpid

#define getpid _getpid


// TODO for every LOG or CHECK i should have a thread_local to re-instantiate the signal-handler if we are on windows
//      not all signal handlers need to be re-installed
namespace {

void signalHandler(int signal_number) {
   using namespace g2::internal;
   std::ostringstream fatal_stream;
   fatal_stream << "\n\n***** FATAL TRIGGER RECEIVED ******* " << std::endl;
   fatal_stream << "\n***** RETHROWING SIGNAL " << signalName(signal_number) << "(" << signal_number << ")" << std::endl;

   LogCapture trigger(FATAL_SIGNAL, signal_number);
   trigger.stream() << fatal_stream.str();
} // scope exit - message sent to LogWorker, wait to die...








} // end anonymous namespace


namespace g2 {
namespace internal {

/// Generate stackdump. Or in case a stackdump was pre-generated and non-empty just use that one 
/// i.e. the latter case is only for Windows and test purposes
std::string stackdump(const char* dump) {
   if (nullptr != dump && !std::string(dump).empty()) {
      return {dump};
   }

   return {"NOT YET IMPLEMENTED"};
}



/// string representation of signal ID
std::string signalName(int signal_number) {
   switch (signal_number) {
   case SIGABRT: return "SIGABRT";
      break;
   case SIGFPE: return "SIGFPE";
      break;
   case SIGSEGV: return "SIGSEGV";
      break;
   case SIGILL: return "SIGILL";
      break;
   case SIGTERM: return "SIGTERM";
      break;
   default:
      std::ostringstream oss;
      oss << "UNKNOWN SIGNAL(" << signal_number << ")";
      return oss.str();
   }
}


// Triggered by g2log::LogWorker after receiving a FATAL trigger
// which is LOG(FATAL), CHECK(false) or a fatal signal our signalhandler caught.
// --- If LOG(FATAL) or CHECK(false) the signal_number will be SIGABRT
void exitWithDefaultSignalHandler(int signal_number) {
   // Restore our signalhandling to default
   if (SIG_ERR == signal(SIGABRT, SIG_DFL))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGFPE, SIG_DFL))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGSEGV, SIG_DFL))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGILL, SIG_DFL))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGTERM, SIG_DFL))
      perror("signal - SIGABRT");

   raise(signal_number);
}

void installSignalHandler() {
   if (SIG_ERR == signal(SIGABRT, signalHandler))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGFPE, signalHandler))
      perror("signal - SIGFPE");
   if (SIG_ERR == signal(SIGSEGV, signalHandler))
      perror("signal - SIGSEGV");
   if (SIG_ERR == signal(SIGILL, signalHandler))
      perror("signal - SIGILL");
   if (SIG_ERR == signal(SIGTERM, signalHandler))
      perror("signal - SIGTERM");
}


} // end g2::internal

void installCrashHandler() {
   internal::installSignalHandler();
}

} // end namespace g2
