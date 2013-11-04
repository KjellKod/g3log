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

   LogMessageBuilder::LogMessageBuilder(const std::string& file, const int line,
           const std::string& function, const LEVELS& level)
   : _message(std::make_shared<LogMessageImpl>(file, line, function, level)) {
   }

   LogMessageBuilder::~LogMessageBuilder() {
      LogMessage log_entry(_message);
      if (log_entry.wasFatal()) {
         FatalMessageBuilder trigger({log_entry.toString(), SIGABRT});
         return; // FatalMessageBuilder will send to worker at scope exit 
      }
      internal::saveMessage(log_entry); // message saved to g2LogWorker
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

   FatalMessageBuilder::FatalMessageBuilder(const std::string& exit_message, int fatal_signal)
   : _exit_message(exit_message), _fatal_signal(fatal_signal) {
   }

   FatalMessageBuilder::~FatalMessageBuilder() {
      // At destruction, flushes fatal message to g2LogWorker
      // either we will stay here until the background worker has received the fatal
      // message, flushed the crash message to the sinks and exits with the same fatal signal
      //..... OR it's in unit-test mode then we throw a std::runtime_error (and never hit sleep)
      FatalMessage msg(_exit_message, _fatal_signal);
      //internal::fatalCall({_exit_message, _fatal_signal});
      internal::fatalCall(msg);

   }
} // g2