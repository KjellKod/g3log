/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================
 *
 * Filename:g2log.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited since it was built on public-domain software and influenced
 * from the following sources
 * 1. kjellkod.cc ;)
 * 2. Dr.Dobbs, Petru Marginean:  http://drdobbs.com/article/printableArticle.jhtml?articleId=201804215&dept_url=/cpp/
 * 3. Dr.Dobbs, Michael Schulze: http://drdobbs.com/article/printableArticle.jhtml?articleId=225700666&dept_url=/cpp/
 * 4. Google 'glog': http://google-glog.googlecode.com/svn/trunk/doc/glog.html
 * 5. Various Q&A at StackOverflow
 * ********************************************* */

#include "g2log.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept> // exceptions
#include <cstdio>    // vsnprintf
#include <cassert>
#include <mutex>

#include "g2logworker.h"
#include "crashhandler.h"
#include <signal.h>

namespace g2
{
namespace constants
{
const int kMaxMessageSize = 2048;
const std::string kTruncatedWarningText = "[...truncated...]";
}
namespace internal
{
static g2LogWorker* g_logger_instance = nullptr; // instantiated and OWNED somewhere else (main)
static std::mutex g_logging_init_mutex;
bool isLoggingInitialized(){return g_logger_instance != nullptr; }

/** thanks to: http://www.cplusplus.com/reference/string/string/find_last_of/
* Splits string at the last '/' or '\\' separator
* example: "/mnt/something/else.cpp" --> "else.cpp"
*          "c:\\windows\\hello.h" --> hello.h
*          "this.is.not-a-path.h" -->"this.is.not-a-path.h" */
std::string splitFileName(const std::string& str)
{
  size_t found;
  found = str.find_last_of("(/\\");
  return str.substr(found+1);
}

} // end namespace g2::internal


void initializeLogging(g2LogWorker *bgworker)
{
  static bool once_only_signalhandler = false;
  std::lock_guard<std::mutex> lock(internal::g_logging_init_mutex);
  CHECK(!internal::isLoggingInitialized());
  CHECK(bgworker != nullptr);
  internal::g_logger_instance = bgworker;

  if(false == once_only_signalhandler)
  {
    installSignalHandler();
    once_only_signalhandler = true;
  }
}

g2LogWorker* shutDownLogging()
{
  std::lock_guard<std::mutex> lock(internal::g_logging_init_mutex);
  CHECK(internal::isLoggingInitialized());
  g2LogWorker *backup = internal::g_logger_instance;
  internal::g_logger_instance = nullptr;
  return backup;
}



namespace internal
{

// The default, initial, handling to send a 'fatal' event to g2logworker
// the caller will stay here, eternally, until the software is aborted
void callFatalInitial(FatalMessage message)
{
  internal::g_logger_instance->fatal(message);
}
// By default this function pointer goes to \ref callFatalInitial;
void (*g_fatal_to_g2logworker_function_ptr)(FatalMessage) = callFatalInitial;



// Replaces the g2log.cpp/g_fatal_to_g2logworker_function_ptr through
// g2log::changeFatalInitHandler
void unitTestFatalInitHandler(g2::internal::FatalMessage fatal_message)
{
  assert(internal::g_logger_instance != nullptr);
  internal::g_logger_instance->save(fatal_message.message_); // calling 'save' instead of 'fatal'
  throw std::runtime_error(fatal_message.message_);
}

// In case of unit-testing - a replacement 'fatal function' can be called
void changeFatalInitHandlerForUnitTesting()
{
  g_fatal_to_g2logworker_function_ptr = unitTestFatalInitHandler;
}





LogContractMessage::LogContractMessage(const std::string &file, const int line,
                                       const std::string& function, const std::string &boolean_expression)
  : LogMessage(file, line, function, "FATAL")
  , expression_(boolean_expression)
{}

LogContractMessage::~LogContractMessage()
{
  std::ostringstream oss;
  if(0 == expression_.compare(k_fatal_log_expression))
  {
    oss << "\n[  *******\tEXIT trigger caused by LOG(FATAL): \n\t";
  }
  else
  {
    oss << "\n[  *******\tEXIT trigger caused by broken Contract: CHECK(" << expression_ << ")\n\t";
  }
  log_entry_ = oss.str();
}

LogMessage::LogMessage(const std::string &file, const int line, const std::string& function, const std::string &level)
  : file_(file)
  , line_(line)
  , function_(function)
  , level_(level)

{}


LogMessage::~LogMessage()
{
  using namespace internal;
  std::ostringstream oss;
  const bool fatal = (0 == level_.compare("FATAL"));
  oss << level_ << " [" << splitFileName(file_);
  if(fatal)
    oss <<  " at: " << function_ ;
  oss << " L: " << line_ << "]\t";

  const std::string str(stream_.str());
  if(!str.empty())
  {
    oss << '"' << str << '"';
  }
  log_entry_ += oss.str();

  if(!isLoggingInitialized() )
  {
    std::cerr << "Did you forget to call g2::InitializeLogging(g2LogWorker*) in your main.cpp?" << std::endl;
    std::cerr << log_entry_ << std::endl << std::flush;
    throw std::runtime_error("Logger not initialized with g2::InitializeLogging(g2LogWorker*) for msg:\n" + log_entry_);
  }


  if(fatal) // os_fatal is handled by crashhandlers
  {
    { // local scope - to trigger FatalMessage sending
      FatalMessage::FatalType fatal_type(FatalMessage::kReasonFatal);
      FatalMessage fatal_message(log_entry_, fatal_type, SIGABRT);
      FatalTrigger trigger(fatal_message);
      std::cerr  << log_entry_ << "\t*******  ]" << std::endl << std::flush;
    } // will send to worker
  }
  internal::g_logger_instance->save(log_entry_); // message saved
}


// represents the actual fatal message
FatalMessage::FatalMessage(std::string message, FatalType type, int signal_id)
  : message_(message)
  , type_(type)
  , signal_id_(signal_id){}

// used to RAII trigger fatal message sending to g2LogWorker
FatalTrigger::FatalTrigger(const FatalMessage &message)
  : message_(message){}

// at destruction, flushes fatal message to g2LogWorker
FatalTrigger::~FatalTrigger()
{
  // either we will stay here eternally, or it's in unit-test mode
  // then we throw a std::runtime_error (and never hit sleep)
  g_fatal_to_g2logworker_function_ptr(message_);
  while(true){std::this_thread::sleep_for(std::chrono::seconds(1));}
}



void LogMessage::messageSave(const char *printf_like_message, ...)
{
  char finished_message[constants::kMaxMessageSize];
  va_list arglist;
  va_start(arglist, printf_like_message);
  const int nbrcharacters = vsnprintf(finished_message, sizeof(finished_message), printf_like_message, arglist);
  va_end(arglist);
  if (nbrcharacters <= 0)
  {
    stream_ << "\n\tERROR LOG MSG NOTIFICATION: Failure to parse successfully the message";
    stream_ << '"' << printf_like_message << '"' << std::endl;
  }
  else if (nbrcharacters > constants::kMaxMessageSize)
  {
    stream_  << finished_message << constants::kTruncatedWarningText;
  }
  else
  {
    stream_ << finished_message;
  }
}

} // end of namespace g2::internal
} // end of namespace g2
