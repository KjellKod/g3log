/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2logmessage.cpp  Part of Framework for Logging and Design By Contract
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "g2logmessage.hpp"
#include <csignal>
#include <stdexcept> // exceptions
#include <thread>
#include <chrono>
#include <mutex>

#include "g2log.hpp"
#include "g2time.hpp"

struct LogEntryMessageImpl
{
  LogEntryMessageImpl(const std::string &file, const int line, const std::string& function,
                      const LEVELS& level, const std::string contract_expression);

  const std::string file_;
  const int line_;
  const std::string function_;
  const LEVELS level_;
  const std::time_t timestamp_;
  const long  microseconds_;
  const std::string message_;
  const std::string expression_; // only with content for CHECK(...) calls

  std::ostringstream stream_;
};
LogEntryMessageImpl::LogEntryMessageImpl(const std::string &file, const int line,
                                         const std::string& function, const LEVELS& level,
                                         const std::string contract_expression)
  : file_(file), line_(line), function_(function), level_(level)
  , timestamp_(g2::systemtime_now())
  , microseconds_(g2::internal::microsecondsCounter())
  , expression_(contract_expression)
{}






std::string LogEntryMessage::line(){return std::to_string(pimpl_->line_);} //ltorreturn {""};} // // pimpl_->line_;
std::string LogEntryMessage::file(){return pimpl_->file_;}
std::string LogEntryMessage::function(){return pimpl_->function_;}
std::string LogEntryMessage::level(){return pimpl_->level_.text;}

std::string LogEntryMessage::timestamp()
{
  using namespace g2;
  std::ostringstream oss;
  // YYYY/MM/DD HH:MM:SS -- ref g2time.hpp/cpp
  oss << localtime_formatted(pimpl_->timestamp_, internal::date_formatted);
  oss << " ";
  oss << localtime_formatted(pimpl_->timestamp_, internal::time_formatted);
  return oss.str();
}

std::string LogEntryMessage::microseconds(){return std::to_string(pimpl_->microseconds_);}
std::string LogEntryMessage::message(){return pimpl_->stream_.str();}
std::string LogEntryMessage::expression(){return pimpl_->expression_;}

std::ostringstream&  LogEntryMessage::saveMessageByStream(){return pimpl_->stream_;}




LogEntryMessage::LogEntryMessage(const std::string &file, const int line, const std::string& function,
                                 const LEVELS& level,  const std::string contract_expression)
  : pimpl_(new LogEntryMessageImpl(file, line, function, level, contract_expression))
{ }


LogEntryMessage::LogEntryMessage(const std::string &file, const int line, const std::string& function,
                                 const LEVELS& level)
  : LogEntryMessage(file, line, function, level, "")
{ }






namespace g2 {
  namespace constants
  {
    const int kMaxMessageSize = 2048;
    const std::string kTruncatedWarningText = "[...truncated...]";
  }

  /**
   Thanks to: http://www.cplusplus.com/reference/string/string/find_last_of/
  * Splits string at the last '/' or '\\' separator
  * example: "/mnt/something/else.cpp" --> "else.cpp"
  *          "c:\\windows\\hello.hpp" --> hello.hpp
  *          "this.is.not-a-path.hpp" -->"this.is.not-a-path.hpp" */
  std::string splitFileName(const std::string& str)
  {
    size_t found;
    found = str.find_last_of("(/\\");
    return str.substr(found+1);
  }

  namespace internal {


    LogMessage::LogMessage(const std::string &file, const int line, const std::string& function, const LEVELS &level)
      : file_(file)
      , line_(line)
      , function_(function)
      , level_(level)
    {}

    // TODO: Madness duh! refactor this till sanity!
    // All logmessage text should be done in the bacground thread
    // Only ONE logmessage? Is needed "expression can just be empty for non-expression types"
    LogMessage::~LogMessage()
    {
      using namespace internal;
      std::ostringstream oss;
      const bool fatal = (FATAL.value == level_.value);
      oss << level_.text << " [" << splitFileName(file_);
      if(fatal)
        oss << "\n[  *******\tEXIT trigger caused by LOG(FATAL): \n\t";

      oss <<  " at: " << function_ ; // previousl only at FATAL.. make configurable but always on FATAL
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
      internal::saveMessage(log_entry_); // message saved to g2LogWorker
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


    LogContractMessage::LogContractMessage(const std::string &file, const int line,
                                           const std::string& function, const std::string &boolean_expression)
      : LogMessage(file, line, function, FATAL)
      , expression_(boolean_expression)
    {}

    LogContractMessage::~LogContractMessage()
    {
      std::ostringstream oss;
      oss << "\n[  *******\tEXIT trigger caused by broken Contract: CHECK(" << expression_ << ")\n\t";
      log_entry_ = oss.str();
    }

  } // internal
} // g2
