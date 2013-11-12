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
      std::string file() const;
      std::string line() const;
      std::string function() const;
      std::string level() const;

      std::string timestamp(const std::string& time_format = {internal::date_formatted + " " + internal::time_formatted}) const;
      std::string microseconds() const;
      std::string message() const;
      std::string expression() const;

      bool wasFatal() const;

      // convert all content to ONE string
      std::string toString() const;


      std::ostringstream& stream();
      void setExpression(const std::string expression);
      
      LogMessage(const std::string &file, const int line, const std::string& function, const LEVELS& level);
      explicit LogMessage(const std::string& fatalOsSignalCrashMessage);
      
      LogMessage(const LogMessage&);
      //LogMessage(LogMessage&&);
      
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
