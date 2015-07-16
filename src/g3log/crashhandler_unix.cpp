/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "g3log/crashhandler.hpp"
#include "g3log/logmessage.hpp"
#include "g3log/logcapture.hpp"
#include "g3log/loglevels.hpp"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) && !defined(__GNUC__)) // windows and not mingw
#error "crashhandler_unix.cpp used but it's a windows system"
#endif

#include <csignal>
#include <cstring>
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <cstdlib>
#include <sstream>
#include <iostream>


// Linux/Clang, OSX/Clang, OSX/gcc
#if (defined(__clang__) || defined(__APPLE__))
#include <sys/ucontext.h>
#else
#include <ucontext.h>
#endif


namespace {
   // Dump of stack,. then exit through g2log background worker
   // ALL thanks to this thread at StackOverflow. Pretty much borrowed from:
   // Ref: http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
   void signalHandler(int signal_number, siginfo_t *info, void *unused_context) {
      using namespace g2::internal;
      {
         const auto dump = stackdump();
         std::ostringstream fatal_stream;
         const auto fatal_reason = exitReasonName(g2::internal::FATAL_SIGNAL, signal_number);
         fatal_stream << "Received fatal signal: " << fatal_reason;
         fatal_stream << "(" << signal_number << ")\tPID: " << getpid() << std::endl;
         fatal_stream << "\n***** SIGNAL " << fatal_reason << "(" << signal_number << ")" << std::endl;
         LogCapture trigger(FATAL_SIGNAL, static_cast<g2::SignalType>(signal_number), dump.c_str());
         trigger.stream() << fatal_stream.str();
      } // message sent to g2LogWorker
      // wait to die
   }
} // end anonymous namespace






// Redirecting and using signals. In case of fatal signals g2log should log the fatal signal
// and flush the log queue and then "rethrow" the signal to exit
namespace g2 {
   // References:
   // sigaction : change the default action if a specific signal is received
   //             http://linux.die.net/man/2/sigaction
   //             http://publib.boulder.ibm.com/infocenter/aix/v6r1/index.jsp?topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf2%2Fsigaction.html
   //
   // signal: http://linux.die.net/man/7/signal and
   //         http://msdn.microsoft.com/en-us/library/xdkz3x12%28vs.71%29.asp
   //
   // memset +  sigemptyset: Maybe unnecessary to do both but there seems to be some confusion here
   //          ,plenty of examples when both or either are used
   //          http://stackoverflow.com/questions/6878546/why-doesnt-parent-process-return-to-the-exact-location-after-handling-signal_number
   namespace internal {

      bool blockForFatalHandling() {
         return true;  // For windows we will after fatal processing change it to false
      }

      /// Generate stackdump. Or in case a stackdump was pre-generated and non-empty just use that one
      /// i.e. the latter case is only for Windows and test purposes
      std::string stackdump(const char *rawdump) {
         if (nullptr != rawdump && !std::string(rawdump).empty()) {
            return {rawdump};
         }

         const size_t max_dump_size = 50;
         void *dump[max_dump_size];
         size_t size = backtrace(dump, max_dump_size);
         char **messages = backtrace_symbols(dump, size); // overwrite sigaction with caller's address

         // dump stack: skip first frame, since that is here
         std::ostringstream oss;
         for (size_t idx = 1; idx < size && messages != nullptr; ++idx) {
            char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;
            // find parantheses and +address offset surrounding mangled name
            for (char *p = messages[idx]; *p; ++p) {
               if (*p == '(') {
                  mangled_name = p;
               } else if (*p == '+') {
                  offset_begin = p;
               } else if (*p == ')') {
                  offset_end = p;
                  break;
               }
            }

            // if the line could be processed, attempt to demangle the symbol
            if (mangled_name && offset_begin && offset_end &&
                  mangled_name < offset_begin) {
               *mangled_name++ = '\0';
               *offset_begin++ = '\0';
               *offset_end++ = '\0';

               int status;
               char *real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
               // if demangling is successful, output the demangled function name
               if (status == 0) {
                  oss << "\n\tstack dump [" << idx << "]  " << messages[idx] << " : " << real_name << "+";
                  oss << offset_begin << offset_end << std::endl;
               }// otherwise, output the mangled function name
               else {
                  oss << "\tstack dump [" << idx << "]  " << messages[idx] << mangled_name << "+";
                  oss << offset_begin << offset_end << std::endl;
               }
               free(real_name); // mallocated by abi::__cxa_demangle(...)
            } else {
               // no demangling done -- just dump the whole line
               oss << "\tstack dump [" << idx << "]  " << messages[idx] << std::endl;
            }
         } // END: for(size_t idx = 1; idx < size && messages != nullptr; ++idx)
         free(messages);
         return oss.str();
      }



      /// string representation of signal ID
      std::string exitReasonName(const LEVELS &level, g2::SignalType fatal_id) {

         int signal_number = static_cast<int>(fatal_id);
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
            oss << "UNKNOWN SIGNAL(" << signal_number << ") for " << level.text;
            return oss.str();
         }
      }



      // KJELL : TODO.  The Fatal Message can contain a callback function that depending on OS and test scenario does
      //       different things.
      // exitWithDefaultSignalHandler is called from g2logworke::bgFatal AFTER all the logging sinks have been cleared
      // I.e. saving a function that has the value already encapsulated within.
      // FatalMessagePtr msgPtr
      // Linux/OSX -->   msgPtr.get()->ContinueWithFatalExit();  --> exitWithDefaultSignalHandler(int signal_number);
      // Windows          .....       (if signal)                --> exitWithDefaultSignalHandler(int signal_number);
      //                              (if exception) ....
      //                              the calling thread that is in a never-ending loop should break out of that loop
      //                                    i.e. an atomic flag should be set
      //                              the next step should then be to re-throw the same exception
      //                              i.e. just call the next exception handler
      //                              we should make sure that 1) g2log exception handler is called BEFORE widows
      //                              it should continue and then be caught in Visual Studios exception handler
      //
      //

      // Triggered by g2log->g2LogWorker after receiving a FATAL trigger
      // which is LOG(FATAL), CHECK(false) or a fatal signal our signalhandler caught.
      // --- If LOG(FATAL) or CHECK(false) the signal_number will be SIGABRT
      void exitWithDefaultSignalHandler(const LEVELS &level, g2::SignalType fatal_signal_id) {
         const int signal_number = static_cast<int>(fatal_signal_id);
         std::cerr << "Exiting due to " << level.text << ", " << signal_number << "   " << std::flush;

#if !(defined(DISABLE_FATAL_SIGNALHANDLING))
         struct sigaction action;
         memset(&action, 0, sizeof (action)); //
         sigemptyset(&action.sa_mask);
         action.sa_handler = SIG_DFL; // take default action for the signal
         sigaction(signal_number, &action, NULL);
#endif

         kill(getpid(), signal_number);
         abort(); // should never reach this
      }
   } // end g2::internal


   //
   // Installs FATAL signal handler that is enough to handle most fatal events
   //  on *NIX systems
   void installSignalHandler() {
#if !(defined(DISABLE_FATAL_SIGNALHANDLING))
      struct sigaction action;
      memset(&action, 0, sizeof (action));
      sigemptyset(&action.sa_mask);
      action.sa_sigaction = &signalHandler; // callback to crashHandler for fatal signals
      // sigaction to use sa_sigaction file. ref: http://www.linuxprogrammingblog.com/code-examples/sigaction
      action.sa_flags = SA_SIGINFO;

      // do it verbose style - install all signal actions
      if (sigaction(SIGABRT, &action, NULL) < 0)
         perror("sigaction - SIGABRT");
      if (sigaction(SIGFPE, &action, NULL) < 0)
         perror("sigaction - SIGFPE");
      if (sigaction(SIGILL, &action, NULL) < 0)
         perror("sigaction - SIGILL");
      if (sigaction(SIGSEGV, &action, NULL) < 0)
         perror("sigaction - SIGSEGV");
      if (sigaction(SIGTERM, &action, NULL) < 0)
         perror("sigaction - SIGTERM");
#endif
   }



   void installCrashHandler() {
      installSignalHandler();
   } // namespace g2::internal


} // end namespace g2

