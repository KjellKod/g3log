/* 
 * File:   MessageBuilder.cpp
 * Author: kjell
 * 
 * Created on October 26, 2013, 3:45 PM
 */

//#include "g2log.hpp"
#include "g2LogMessageBuilder.hpp"
#include "g2logmessageimpl.hpp"
#include "g2logmessage.hpp"
#include <csignal>

namespace {
   const int kMaxMessageSize = 2048;
   const std::string kTruncatedWarningText = "[...truncated...]";
}
namespace g2 {
   using namespace internal;
   
   LogMessageBuilder::LogMessageBuilder(const std::string& file, const int line,
           const std::string& function, const LEVELS& level)
   : _message(std::make_shared<LogMessageImpl>(file, line, function, level)) {
   }

   LogMessageBuilder::~LogMessageBuilder() {
      if (internal::wasFatal(_message->_level)) {
         FatalMessageBuilder trigger({_message, SIGABRT});
         return; // FatalMessageBuilder will send to worker at scope exit 
      }

      LogMessage log_entry(_message);
      saveMessage(log_entry); // message saved to g2LogWorker
   }

   LogMessageBuilder& LogMessageBuilder::setExpression(const std::string& boolean_expression) {
      _message->_expression = boolean_expression;
      return *this;
   }

   std::ostringstream& LogMessageBuilder::stream() {
      return _message->_stream;
   }

   void LogMessageBuilder::messageSave(const char *printf_like_message, ...) {
      char finished_message[kMaxMessageSize];
      va_list arglist;
      va_start(arglist, printf_like_message);
      const int nbrcharacters = vsnprintf(finished_message, sizeof (finished_message), printf_like_message, arglist);
      va_end(arglist);
      if (nbrcharacters <= 0) {
         stream() << "\n\tERROR LOG MSG NOTIFICATION: Failure to parse successfully the message";
         stream() << '"' << printf_like_message << '"' << std::endl;
      } else if (nbrcharacters > kMaxMessageSize) {
         stream() << finished_message << kTruncatedWarningText;
      } else {
         stream() << finished_message;
      }
   }





   /// FatalMessageBuilder

   FatalMessageBuilder::FatalMessageBuilder(const std::string& exit_message, int signal_id)
   : _fatal_message(std::make_shared<LogMessageImpl>(exit_message)), _fatal_signal(signal_id) 
   { }

   
  FatalMessageBuilder:: FatalMessageBuilder(std::shared_ptr<LogMessageImpl> details, int signal_id)
  : _fatal_message(details), _fatal_signal(signal_id) 
  {}
     

   FatalMessageBuilder::~FatalMessageBuilder() {
      // At destruction, flushes fatal message to g2LogWorker
      // either we will stay here until the background worker has received the fatal
      // message, flushed the crash message to the sinks and exits with the same fatal signal
      //..... OR it's in unit-test mode then we throw a std::runtime_error (and never hit sleep)
      FatalMessage msg(_fatal_message, _fatal_signal);
      fatalCall(msg);

   }
} // g2