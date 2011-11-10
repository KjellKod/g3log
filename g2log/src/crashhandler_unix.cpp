#include "crashhandler.h"
#include "g2log.h"

#include <csignal>
#include <cstring>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// or? gcc on windows I guess,. 
#error "crashhandler_unix.cpp used but it's a windows system"
#endif


#include <unistd.h>  // getpid,
#include <execinfo.h>
#include <ucontext.h>
#include <cstdlib>

namespace
{
// simple dump of stack,. then exit through g2log background worker
// Ref: http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
void crashHandler(int signal_number, siginfo_t *info, void *unused_context)
{
  const size_t max_dump_size = 100;
  void* dump[max_dump_size];
  size_t size = backtrace(dump, max_dump_size);
  // overwrite sigaction with caller's address
  char** messages = backtrace_symbols(dump, size);

  std::ostringstream oss;
  oss << "Received fatal signal: " << g2::internal::signalName(signal_number);
  oss << "(" << signal_number << ")" << std::endl;
  oss << "\tPID: " << getpid() << std::endl;
  // dump stack: skip first frame, since that is here
  for(size_t idx = 1; idx < size && messages != nullptr; ++idx)
  {
    oss << "\tstack dump [" << idx << "]  " << messages[idx] << std::endl;
  }
  free(messages);
  { // Local scope, trigger send
    using namespace g2::internal;
    std::ostringstream fatal_stream;
    fatal_stream << "\n\n***** FATAL TRIGGER RECEIVED ******* " << std::endl;
    fatal_stream << oss.str() << std::endl;
    fatal_stream << "\n***** RETHROWING SIGNAL " << signalName(signal_number) << "(" << signal_number << ")" << std::endl;

    FatalMessage fatal_message(fatal_stream.str(),FatalMessage::kReasonOS_FATAL_SIGNAL, signal_number);
    FatalTrigger trigger(fatal_message);  std::ostringstream oss;
    std::cerr << fatal_message.message_ << std::endl << std::flush;
  } // message sent to LogWorker
  // wait to die -- will be inside the FatalTrigger
}
} // end anonymous namespace



// Redirecting and using signals. In case of fatal signals g2log should log the fatal signal
// and flush the log queue and then "rethrow" the signal to exit
namespace g2
{
// References:
// sigaction : change the default action if a specific signal is received
//             http://linux.die.net/man/2/sigaction
//             http://publib.boulder.ibm.com/infocenter/aix/v6r1/index.jsp?topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf2%2Fsigaction.html
//
// signal: http://linux.die.net/man/7/signal and
//         http://msdn.microsoft.com/en-us/library/xdkz3x12%28vs.71%29.asp
//
// memset +  sigemptyset: Maybe unnecessary to do both but there seems to be some confusion here
//                      ,plenty of examples when both or either are used
//          http://stackoverflow.com/questions/6878546/why-doesnt-parent-process-return-to-the-exact-location-after-handling-signal_number
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
  struct sigaction action;
  memset(&action, 0, sizeof(action));  //
  sigemptyset(&action.sa_mask);
  action.sa_handler = SIG_DFL; // take default action for the signal
  sigaction(signal_number, &action, NULL);
  kill(getpid(), signal_number);
}
} // end g2::internal


void installSignalHandler()
{
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  sigemptyset(&action.sa_mask);
  action.sa_sigaction = &crashHandler;  // callback to crashHandler for fatal signals
  // sigaction to use sa_sigaction file. ref: http://www.linuxprogrammingblog.com/code-examples/sigaction
  action.sa_flags = SA_SIGINFO;

  // do it verbose style - install all signal actions
  if(sigaction(SIGABRT, &action, NULL) < 0)
    perror("sigaction - SIGABRT");
  if(sigaction(SIGFPE, &action, NULL) < 0)
    perror("sigaction - SIGFPE");
  if(sigaction(SIGILL, &action, NULL) < 0)
    perror("sigaction - SIGILL");
  if(sigaction(SIGSEGV, &action, NULL) < 0)
    perror("sigaction - SIGSEGV");
  if(sigaction(SIGTERM, &action, NULL) < 0)
    perror("sigaction - SIGTERM");
}
} // end namespace g2
