/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#pragma once
#include <string>
#include <sstream>
#include <cstdarg>

#include "g2loglevels.hpp"
#include "crashhandler.hpp"
#include "g2log.hpp"

struct LogCapture {
   /// Called from crash handler when a fatal signal has occurred (SIGSEGV etc)
   LogCapture(const LEVELS& level, int fatal_signal)
   : LogCapture("", 0, "", level, "", fatal_signal) {
   }

   /**
    * Simple struct for capturing log/fatal entries. At destruction the captured message is forwarded to background worker. 
    * -- As a safety precaution: No memory allocated here will be moved into the background worker in case of dynamic loaded library reasons
    * --    instead the arguments are copied inside of g2log.cpp::saveMessage
    * @file, line, function are given in g2log.hpp from macros
    * @level INFO/DEBUG/WARNING/FATAL
    * @expression for CHECK calls
    * @fatal_signal for failed CHECK:SIGABRT or fatal signal caught in the signal handler
    */
   LogCapture(const char* file, const int line, const char* function, const LEVELS& level, const char* expression = "", int fatal_signal = SIGABRT)
   : _file(file), _line(line), _function(function), _level(level), _expression(expression), _fatal_signal(fatal_signal) {

      if (g2::internal::wasFatal(level)) {
         _stack_trace = {"\n*******\tSTACKDUMP *******\n"};
         _stack_trace.append(g2::internal::stackdump());
      }
   }


   // At destruction the message will be forwarded to the g2log worker. 
   // in case of dynamically (at runtime) loaded libraries the important thing to know is that 
   // all strings are copied so the original are not destroyed at the receiving end, only the copy
   ~LogCapture();




   // Use "-Wall" to generate warnings in case of illegal printf format. 
   //      Ref:  http://www.unixwiz.net/techtips/gnu-c-attributes.html
#ifndef __GNUC__
#define  __attribute__(x) // Disable 'attributes' if compiler does not support 'em
#endif
   
   void capturef(const char *printf_like_message, ...) __attribute__((format(printf, 2, 3))) // ref:  http://www.codemaestro.com/reviews/18
   {
      static const int kMaxMessageSize = 2048;
      static const std::string kTruncatedWarningText = "[...truncated...]";
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

   std::ostringstream& stream() {
      return _stream;
   }



   std::ostringstream _stream;
   std::string _stack_trace;
   const char* _file;
   const int _line;
   const char* _function;
   const LEVELS& _level;
   const char* _expression;
   const int _fatal_signal;

};
//} // g2
