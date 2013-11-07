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
#include "g2logmessageimpl.hpp"
#include "g2time.hpp"
#include "crashhandler.hpp"
#include <stdexcept> // exceptions
#include "g2log.hpp"


namespace g2 {


   std::string LogMessage::line() const {
      return std::to_string(_pimpl->_line);
   }


   std::string LogMessage::file() const {
      return _pimpl->_file;
   }


   std::string LogMessage::function() const {
      return _pimpl->_function;
   }


   std::string LogMessage::level() const {
      return _pimpl->_level.text;
   }


   std::string LogMessage::timestamp(const std::string & time_look) const {
      std::ostringstream oss;
      oss << localtime_formatted(_pimpl->_timestamp, time_look);
      return oss.str();
   }


   std::string LogMessage::microseconds() const {
      return std::to_string(_pimpl->_microseconds);
   }


   std::string LogMessage::message() const {
      return _pimpl->_stream.str();
   }


   std::string LogMessage::expression() const {
      return _pimpl->_expression;
   }


   bool LogMessage::wasFatal() const {
      return internal::wasFatal(_pimpl->_level);
   }

   std::string LogMessage::toString() const {
      std::ostringstream oss;
      oss << "\n" << timestamp() << "." << microseconds() << "\t";


      oss << level() << " [" << file();
      oss << " L: " << line() << "]\t";

      // Non-fatal Log Message
      if (false == wasFatal()) {
         oss << '"' << message() << '"';
         return oss.str();
      }

      if (internal::FATAL_SIGNAL.value == _pimpl->_level.value) {
         oss.str(""); // clear any previous text and formatting
         oss << "\n" << timestamp() << "." << microseconds();
         oss << "\n\n***** FATAL SIGNAL RECEIVED ******* " << std::endl;
         oss << '"' << message() << '"';
         return oss.str();
      }


      // Not crash scenario but LOG or CONTRACT
      auto level_value = _pimpl->_level.value;
      if (FATAL.value == level_value) {
         oss << "\n\t*******\tEXIT trigger caused LOG(FATAL) entry: \n\t";
         oss << '"' << message() << '"';
         oss << "\n*******\tSTACKDUMP *******\n" << internal::stackdump();
      } else if (internal::CONTRACT.value == level_value) {
         oss << "\n\t  *******\tEXIT trigger caused by broken Contract: CHECK(" << _pimpl->_expression << ")\n\t";
         oss << '"' << message() << '"';
         oss << "\n*******\tSTACKDUMP *******\n" << internal::stackdump()<< '"';
      } else {
         oss << "\n\t*******\tUNKNOWN Log Message Type\n" << '"' << message() << '"';
      }

      return oss.str();
   }


   std::ostringstream& LogMessage::stream() {
      return _pimpl->_stream;
   }


   LogMessage::LogMessage(std::shared_ptr<LogMessageImpl> details)
   : _pimpl(details) { }


   FatalMessage::FatalMessage(std::shared_ptr<LogMessageImpl> details, int signal_id) 
   : LogMessage(details), signal_id_(signal_id) { }

   LogMessage  FatalMessage::copyToLogMessage() const {
      return LogMessage(_pimpl);
   }


} // g2
