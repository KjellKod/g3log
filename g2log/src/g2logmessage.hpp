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
#include <iostream>
#include <cstdarg>
#include <memory>

#include "g2log.hpp"
#include "g2loglevels.hpp"
#include "g2time.hpp"
//#include "g2logmessageimpl.hpp"

namespace g2 {
   struct LogMessageImpl;

   struct LogMessage {
      mutable std::shared_ptr<LogMessageImpl> _pimpl;
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


      std::ostringstream& stream() const;
      explicit LogMessage(const std::shared_ptr<LogMessageImpl>& details);
      ~LogMessage() = default;
   };

   /** Trigger for flushing the message queue and exiting the application
    * A thread that causes a FatalMessage will sleep forever until the
    * application has exited (after message flush) */
   struct FatalMessage : public LogMessage {
      FatalMessage(const std::shared_ptr<LogMessageImpl>& details, int signal_id);
      
      ~FatalMessage() = default;
      LogMessage copyToLogMessage() const;
      
      int signal_id_;
   };
} // g2
