/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#error "crashhandler_windows.cpp used but not on a windows system"
#endif

#include <windows.h>

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include <process.h> // getpid
#define getpid _getpid

#include "crashhandler.hpp"
#include "stacktrace_windows.hpp"
#include "g2logmessage.hpp"
#include "g2logmessagecapture.hpp"
#include <working_trace.hpp>

#include <iostream>  // TODO REMOVE


// TODO for every LOG or CHECK i should have a thread_local to re-instantiate the signal-handler if we are on windows
//      not all signal handlers need to be re-installed
namespace {

void signalHandler(int signal_number) {
   using namespace g2::internal;
   std::ostringstream fatal_stream;
   fatal_stream << "\n\n***** FATAL TRIGGER RECEIVED ******* " << std::endl;
   fatal_stream << "\n***** RETHROWING SIGNAL " << signalName(signal_number) << "(" << signal_number << ")" << std::endl;

   //const auto dump = stackdump();
  /***********    START HACK ************/
  CONTEXT current_context;
  memset(&current_context, 0, sizeof(CONTEXT));
  RtlCaptureContext(&current_context);  
  stack_trace sttrace(&current_context);
  const auto dump = sttrace.to_string();
  /************ STOP HACk ***********/

   LogCapture trigger(FATAL_SIGNAL, signal_number, dump.c_str());
   trigger.stream() << fatal_stream.str();
} // scope exit - message sent to LogWorker, wait to die...



//
// Unhandled exception catching
LONG WINAPI exceptionHandling(EXCEPTION_POINTERS* info) {
   const auto exception_code = info->ExceptionRecord->ExceptionCode;
   //const auto exceptionText = stacktrace::exceptionIdToText(exception_code);
   /**** START HACK   ****/
   stack_trace sttrace(info->ContextRecord);     
   // if there is a windows exception then call it like THIS
   auto exceptionText = sttrace.to_string();

   /**** STOP HACK ***/
   //std::cout << "\nexceptionHandling: " << __FUNCTION__ << ", received: " << exceptionText << std::endl;
   LogCapture trigger(g2::internal::FATAL_EXCEPTION, SIGABRT /*LogCapture::kExceptionAndNotASignal*/, exceptionText.c_str());
  // TODO .... after exit it should NOT throw SIGABRT
  // it should instead make sure that it stopps blocking forever
  return EXCEPTION_CONTINUE_SEARCH; //EXCEPTION_EXECUTE_HANDLER;
}


/// Setup through (Windows API) AddVectoredExceptionHandler
/// Running exceptions in a Windows debugger might make it impossible to hit the exception handlers
/// Ref: http://blogs.msdn.com/b/zhanli/archive/2010/06/25/c-tips-addvectoredexceptionhandler-addvectoredcontinuehandler-and-setunhandledexceptionfilter.aspx
LONG WINAPI vectorExceptionHandling(PEXCEPTION_POINTERS p) {
   std::cout << "\nIn my vectored exception_filter: " << __FUNCTION__ << std::endl;
   return exceptionHandling(p);
}






} // end anonymous namespace


namespace g2 {
namespace internal {

/// Generate stackdump. Or in case a stackdump was pre-generated and non-empty just use that one
/// i.e. the latter case is only for Windows and test purposes
std::string stackdump(const char *dump) {
   if (nullptr != dump && !std::string(dump).empty()) {
      return {dump};
   }

   return stacktrace::stackdump();
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
   // TODO,. instead of existing at fatal incidents we should catch it
   // flush all to logs,.   call ShutdownLogging(?)  and re-throw the signal
   // or the exception  (i.e .the normal )
   internal::installSignalHandler();
   //const size_t kFirstExceptionHandler = 1;
   const size_t kLastExceptionHandler = 0;
   // http://msdn.microsoft.com/en-us/library/windows/desktop/ms679274%28v=vs.85%29.aspx
   AddVectoredExceptionHandler(kLastExceptionHandler, vectorExceptionHandling);
   SetUnhandledExceptionFilter(exceptionHandling);


}

} // end namespace g2
