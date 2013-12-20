/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#include "g2LogMessageBuilder.hpp"
#include "g2logmessage.hpp"
#include "g2log.hpp"
#include "std2_make_unique.hpp"
#include "crashhandler.hpp"
#include <csignal>

#include <iostream>
namespace {
   const int kMaxMessageSize = 2048;
   const std::string kTruncatedWarningText = "[...truncated...]";
}
namespace g2 {
   using namespace internal;
   
   LogMessageBuilder::LogMessageBuilder(const std::string& file, const int line,
           const std::string& function, const LEVELS& level)
   : _message(std2::make_unique<LogMessage>(file, line, function, level)) {
      
      if (_message.get()->wasFatal()) {
         addStackTrace();
      }
   }

   LogMessageBuilder::~LogMessageBuilder() {
      _message.get()->write().append(stream().str());
      if (_message.get()->wasFatal()) {
         _message.get()->write().append(_stackTrace); // empty or not
         FatalMessageBuilder trigger(_message, SIGABRT);
         return; // FatalMessageBuilder will send to worker at scope exit 
      }

      saveMessage(_message); // message saved to g2LogWorker
   }

   LogMessageBuilder& LogMessageBuilder::setExpression(const std::string& boolean_expression) {
     _message.get()->setExpression(boolean_expression);
      return *this;
   }

   LogMessageBuilder& LogMessageBuilder::addStackTrace() {
      _stackTrace = {"\n*******\tSTACKDUMP *******\n"};
      _stackTrace.append(internal::stackdump()); 
      return *this;
   }


   std::ostringstream& LogMessageBuilder::stream() {
      return _stream;
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
   : _fatal_message(std2::make_unique<FatalMessage>(LogMessage{exit_message}, signal_id)) 
   { }

   
  FatalMessageBuilder:: FatalMessageBuilder(LogMessagePtr details, int signal_id)
     : _fatal_message(std2::make_unique<FatalMessage>(*(details._move_only.get()), signal_id)) 
  {}
     

   FatalMessageBuilder::~FatalMessageBuilder() {
      // At destruction, flushes fatal message to g2LogWorker
      // either we will stay here until the background worker has received the fatal
      // message, flushed the crash message to the sinks and exits with the same fatal signal
      //..... OR it's in unit-test mode then we throw a std::runtime_error (and never hit sleep)
      
      fatalCall(_fatal_message);

   }
} // g2
