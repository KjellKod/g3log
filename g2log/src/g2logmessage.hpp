/** ==========================================================================
 * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================
 * Filename:g2logmessage.hpp  Part of Framework for Logging and Design By Contract
 * Created: 2012 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#pragma once


#include <string>
#include <sstream>
#include <chrono>

#include "g2loglevels.hpp"
#include "g2time.hpp"
#include "g2pretendtobecopyable.hpp"
#include <memory>

namespace g2 {
   struct LogMessage {
      std::string file() const { return _file; }
      std::string line() const { return std::to_string(_line); }
      std::string function() const { return _function; }
      std::string level() const { return _level.text; }

      std::string timestamp(const std::string& time_format = {internal::date_formatted + " " + internal::time_formatted}) const;
      std::string microseconds() const { return std::to_string(_microseconds); }
      std::string message() const {  return _stream.str(); }
      std::string expression() const { return _expression; }
      bool wasFatal() const { return internal::wasFatal(_level); }

      std::string toString() const;


      std::ostringstream& stream() { return _stream; }
      void setExpression(const std::string expression) { _expression = expression; } 
      
      LogMessage(const std::string &file, const int line, const std::string& function, const LEVELS& level);
      explicit LogMessage(const std::string& fatalOsSignalCrashMessage);
     
      LogMessage(const LogMessage&);      
      ~LogMessage() = default;
      
   protected:
      const std::time_t _timestamp;
      const long _microseconds;
      const std::string _file;
      const int _line;
      const std::string _function;
      const LEVELS _level;

      std::string _expression; // only with content for CHECK(...) calls
      mutable std::ostringstream _stream;
   };
   
   
   

   /** Trigger for flushing the message queue and exiting the application
    * A thread that causes a FatalMessage will sleep forever until the
    * application has exited (after message flush) */
   struct FatalMessage : public LogMessage {
      FatalMessage(const LogMessage& details, int signal_id);
      FatalMessage(const FatalMessage&);
      ~FatalMessage() = default;
      
      LogMessage copyToLogMessage() const;
      std::string signal() const;
      
      const int _signal_id;
   };
   
   
   typedef PretendToBeCopyable<std::unique_ptr<FatalMessage>> FatalMessagePtr;
   typedef PretendToBeCopyable<std::unique_ptr<LogMessage>> LogMessagePtr;
} // g2
