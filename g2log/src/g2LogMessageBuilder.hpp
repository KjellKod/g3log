/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#pragma once
#include <string>
#include <sstream>
#include <cstdarg>

#include "g2loglevels.hpp"
#include "g2logmessage.hpp"

namespace g2 {

   // At RAII scope end this struct will trigger a FatalMessage sending
   struct FatalMessageBuilder {
      //explicit FatalMessageBuilder(const FatalMessage& exit_message);
      FatalMessageBuilder(const std::string& exit_message, int fatal_signal);
      FatalMessageBuilder(LogMessagePtr details, int signal_id);
      virtual ~FatalMessageBuilder();

      FatalMessagePtr _fatal_message;
      int _fatal_signal;
   };

   struct LogMessageBuilder {
      LogMessageBuilder(const std::string& file, const int line, const std::string& function, const LEVELS& level);
      virtual ~LogMessageBuilder();

      LogMessageBuilder& setExpression(const std::string& boolean_expression);
      LogMessageBuilder& addStackTrace();

      std::ostringstream& stream();
      
   
      // Use "-Wall" to generate warnings in case of illegal printf format. 
      //      Ref:  http://www.unixwiz.net/techtips/gnu-c-attributes.html
#ifndef __GNUC__
#define  __attribute__(x) // Disable 'attributes' if compiler does not support 'em
#endif
      void messageSave(const char *printf_like_message, ...)
      __attribute__((format(printf, 2, 3))); // ref:  http://www.codemaestro.com/reviews/18

   private:
      LogMessagePtr _message;
      std::ostringstream _stream;
      std::string _stackTrace;
   };
} // g2
