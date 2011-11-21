/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#include "crashhandler.h"
#include "g2log.h"

#include <csignal>
#include <cstring>
#include <cstdlib>
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#error "crashhandler_win.cpp used but not on a windows system"
#endif

#include <process.h> // getpid
#define getpid _getpid

namespace
{
void crashHandler(int signal_number)
{
    using namespace g2::internal;
    std::ostringstream fatal_stream;
    fatal_stream << "\n\n***** FATAL TRIGGER RECEIVED ******* " << std::endl;
    fatal_stream << "\n***** RETHROWING SIGNAL " << signalName(signal_number) << "(" << signal_number << ")" << std::endl;

    FatalMessage fatal_message(fatal_stream.str(),FatalMessage::kReasonOS_FATAL_SIGNAL, signal_number);
    FatalTrigger trigger(fatal_message);  std::ostringstream oss;
    std::cerr << fatal_message.message_ << std::endl << std::flush;
} // scope exit - message sent to LogWorker, wait to die...
} // end anonymous namespace


namespace g2
{
namespace internal
{
std::string signalName(int signal_number)
{
  switch(signal_number)
  {
  case SIGABRT: return "SIGABRT";break;
  case SIGFPE:  return "SIGFPE"; break;
  case SIGSEGV: return "SIGSEGV"; break;
  case SIGILL:  return "SIGILL"; break;
  case SIGTERM: return "SIGTERM"; break;
default:
    std::ostringstream oss;
    oss << "UNKNOWN SIGNAL(" << signal_number << ")";
    return oss.str();
  }
}


// Triggered by g2log::LogWorker after receiving a FATAL trigger
// which is LOG(FATAL), CHECK(false) or a fatal signal our signalhandler caught.
// --- If LOG(FATAL) or CHECK(false) the signal_number will be SIGABRT
void exitWithDefaultSignalHandler(int signal_number)
{
    // Restore our signalhandling to default
	    if(SIG_ERR == signal (SIGABRT, SIG_DFL))	
		perror("signal - SIGABRT");
	if(SIG_ERR == signal (SIGFPE, SIG_DFL))
				perror("signal - SIGABRT");
	if(SIG_ERR == signal (SIGSEGV, SIG_DFL))
				perror("signal - SIGABRT");
	if(SIG_ERR == signal (SIGILL, SIG_DFL))
				perror("signal - SIGABRT");
	if(SIG_ERR == signal (SIGTERM, SIG_DFL))
				perror("signal - SIGABRT");
	
   raise(signal_number);
}
} // end g2::internal


void installSignalHandler()
{
    if(SIG_ERR == signal (SIGABRT, crashHandler))	
		perror("signal - SIGABRT");
	if(SIG_ERR == signal (SIGFPE, crashHandler))
				perror("signal - SIGFPE");
	if(SIG_ERR == signal (SIGSEGV, crashHandler))
				perror("signal - SIGSEGV");
	if(SIG_ERR == signal (SIGILL, crashHandler))
				perror("signal - SIGILL");
	if(SIG_ERR == signal (SIGTERM, crashHandler))
				perror("signal - SIGTERM");
}
} // end namespace g2
