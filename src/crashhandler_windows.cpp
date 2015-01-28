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
#include <atomic>

#include <process.h> // getpid
#define getpid _getpid

#include "crashhandler.hpp"
#include "stacktrace_windows.hpp"
#include "g2logmessage.hpp"
#include "g2logmessagecapture.hpp"
#include "working_trace.hpp"

#include <iostream>  // TODO REMOVE


// TODO for every LOG or CHECK i should have a thread_local to re-instantiate the signal-handler if we are on windows
//      not all signal handlers need to be re-installed
namespace {
std::atomic<bool> gBlockForFatal {true};
void* g_vector_exception_handler = nullptr;
LPTOP_LEVEL_EXCEPTION_FILTER g_previous_unexpected_exception_handler = nullptr;

// Restore back to default fatal event handling
void ReverseToOriginalFatalHandling() {
   SetUnhandledExceptionFilter (g_previous_unexpected_exception_handler);
   RemoveVectoredExceptionHandler (g_vector_exception_handler);
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
}



void signalHandler(int signal_number) {
   using namespace g2::internal;
   std::ostringstream fatal_stream;
   fatal_stream << "\n\n***** FATAL TRIGGER RECEIVED ******* " << std::endl;
   fatal_stream << "\n***** RETHROWING SIGNAL " << exitReasonName(g2::internal::FATAL_SIGNAL, signal_number) << "(" << signal_number << ")" << std::endl;

   //const auto dump = stackdump();
   /***********    START HACK ************/
   CONTEXT current_context;
   memset(&current_context, 0, sizeof(CONTEXT));
   RtlCaptureContext(&current_context);
   stack_trace sttrace(&current_context);
   const auto dump = sttrace.to_string();
   /************ STOP HACk ***********/

   LogCapture trigger(FATAL_SIGNAL, static_cast<g2::SignalType>(signal_number), dump.c_str());
   trigger.stream() << fatal_stream.str();
} // scope exit - message sent to LogWorker, wait to die...



//
// Unhandled exception catching
LONG WINAPI exceptionHandling(EXCEPTION_POINTERS* info) {
   const g2::SignalType exception_code = info->ExceptionRecord->ExceptionCode;
   //const auto exceptionText = stacktrace::exceptionIdToText(exception_code);
   /**** START HACK   ****/
   stack_trace sttrace(info->ContextRecord);
   // if there is a windows exception then call it like THIS
   auto exceptionText = sttrace.to_string();
   const auto fatal_id = static_cast<g2::SignalType>(exception_code);

   /**** STOP HACK ***/
   std::cout << __FUNCTION__ << " " << __LINE__ << "exception id:  " << fatal_id
             << ", windows exc: " << stacktrace::exceptionIdToText(fatal_id) <<    std::endl;

   LogCapture trigger(g2::internal::FATAL_EXCEPTION, static_cast<g2::SignalType>(exception_code), exceptionText.c_str());
   return EXCEPTION_CONTINUE_SEARCH; //EXCEPTION_EXECUTE_HANDLER;
}






//
// Unhandled exception catching
LONG WINAPI unexpectedExceptionHandling(EXCEPTION_POINTERS* info) {
   ReverseToOriginalFatalHandling();
   return exceptionHandling(info);
}


/// Setup through (Windows API) AddVectoredExceptionHandler
/// Running exceptions in a Windows debugger might make it impossible to hit the exception handlers
/// Ref: http://blogs.msdn.com/b/zhanli/archive/2010/06/25/c-tips-addvectoredexceptionhandler-addvectoredcontinuehandler-and-setunhandledexceptionfilter.aspx
LONG WINAPI vectorExceptionHandling(PEXCEPTION_POINTERS p) {
   std::cout << "\nIn my vectored exception_filter: " << __FUNCTION__ << std::endl;
   ReverseToOriginalFatalHandling();
   return exceptionHandling(p);
}





} // end anonymous namespace


namespace g2 {
namespace internal {


// For windows exceptions this might ONCE be set to false, in case of a
// windows exceptions and not a signal
bool blockForFatalHandling() {
   return gBlockForFatal;
}


/// Generate stackdump. Or in case a stackdump was pre-generated and non-empty just use that one
/// i.e. the latter case is only for Windows and test purposes
std::string stackdump(const char* dump) {
   if (nullptr != dump && !std::string(dump).empty()) {
      return {dump};
   }

   return stacktrace::stackdump();
}



/// string representation of signal ID or Windows exception id
std::string exitReasonName(const LEVELS& level, g2::SignalType fatal_id) {
   //
   std::cout << __FUNCTION__ << " exit reason: " << fatal_id << std::endl;
   if (level == g2::internal::FATAL_EXCEPTION) {
      return stacktrace::exceptionIdToText(fatal_id);
   }

   switch (fatal_id) {
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
      oss << "UNKNOWN SIGNAL(" << fatal_id << ")";
      return oss.str();
   }
}


// Triggered by g2log::LogWorker after receiving a FATAL trigger
// which is LOG(FATAL), CHECK(false) or a fatal signal our signalhandler caught.
// --- If LOG(FATAL) or CHECK(false) the signal_number will be SIGABRT
void exitWithDefaultSignalHandler(const LEVELS& level, g2::SignalType fatal_signal_id) {

   ReverseToOriginalFatalHandling();
   // For windows exceptions we want to continue the possibility of exception handling
   // now when the log and stacktrace are flushed to sinks. We therefore avoid to kill
   // the preocess here. Instead it will be the exceptionHandling functions above that
   // will let exception handling continue with: EXCEPTION_CONTINUE_SEARCH
   if (g2::internal::FATAL_EXCEPTION == level) {
      gBlockForFatal = false;
      return;
   }


   const int signal_number = static_cast<int>(fatal_signal_id);
   raise(signal_number);
}

void installSignalHandler() {
   g2::installSignalHandlerForThread();

   if (SIG_ERR == signal(SIGABRT, signalHandler))
      perror("signal - SIGABRT");
   if (SIG_ERR == signal(SIGTERM, signalHandler))
      perror("signal - SIGTERM");
}


} // end g2::internal


///  SIGFPE, SIGILL, and SIGSEGV handling must be installed per thread
/// on Windows. This is automatically done if you do at least one LOG(...) call
/// you can also use this function call, per thread so make sure these three
/// fatal signals are covered in your thread (even if you don't do a LOG(...) call
void installSignalHandlerForThread() {
   if (SIG_ERR == signal(SIGFPE, signalHandler))
      perror("signal - SIGFPE");
   if (SIG_ERR == signal(SIGSEGV, signalHandler))
      perror("signal - SIGSEGV");
   if (SIG_ERR == signal(SIGILL, signalHandler))
      perror("signal - SIGILL");

}

void installCrashHandler() {
   internal::installSignalHandler();
   //const size_t kFirstExceptionHandler = 1;   // Kept here for documentational purposes. last exception seems more what we want
   const size_t kLastExceptionHandler = 0;
   // http://msdn.microsoft.com/en-us/library/windows/desktop/ms679274%28v=vs.85%29.aspx
   g_vector_exception_handler = AddVectoredExceptionHandler(kLastExceptionHandler, vectorExceptionHandling);
   g_previous_unexpected_exception_handler = SetUnhandledExceptionFilter(unexpectedExceptionHandling);
}

} // end namespace g2
