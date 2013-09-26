#ifndef CRASH_HANDLER_H_
#define CRASH_HANDLER_H_
/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*/
#include <string>
#include <csignal>

namespace g2 {
namespace internal {
/// @return signal_name. Ref: signum.h and @ref installSignalHandler */
std::string signalName(int signal_number);

/** Re-"throw" a fatal signal, previously caught. This will exit the application
 * This is an internal only function. Do not use it elsewhere. It is triggered
 * from g2log, g2LogWorker after flushing messages to file */
void exitWithDefaultSignalHandler(int signal_number);


// PUBLIC API:
/** Install signal handler that catches FATAL C-runtime or OS signals
    SIGABRT  ABORT (ANSI), abnormal termination
    SIGFPE   Floating point exception (ANSI): http://en.wikipedia.org/wiki/SIGFPE
    SIGILL   ILlegal instruction (ANSI)
    SIGSEGV  Segmentation violation i.e. illegal memory reference
    SIGTERM  TERMINATION (ANSI) */
void installSignalHandler();

} // g2::internal
} // g2

#endif // CRASH_HANDLER_H_
