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
#include "crashhandler.hpp"
#include "g2time.hpp"
#include "std2_make_unique.hpp"
#include <mutex>

namespace {
   std::once_flag g_start_time_flag;
   g2::steady_time_point g_start_time;

   long microsecondsCounter() {
      std::call_once(g_start_time_flag, []() { g_start_time =  std::chrono::steady_clock::now();  });
      g2::steady_time_point  now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::microseconds>(now - g_start_time).count();
   }

   std::string splitFileName(const std::string& str) {
      size_t found;
      found = str.find_last_of("(/\\");
      return str.substr(found + 1);
   }
} // anonymous



namespace g2 {

//   std::string LogMessage::toString() const {
//      std::ostringstream oss;
//      oss << "\n" << timestamp() << "." << microseconds() << "\t";
//      oss << level() << " [" << file();
//      oss << " L: " << line() << "]\t";
//
//      // Non-fatal Log Message
//      if (false == wasFatal()) {
//         oss << '"' << message() << '"';
//         return oss.str();
//      }
//
//      if (internal::FATAL_SIGNAL.value == _level.value) {
//         oss.str(""); // clear any previous text and formatting
//         oss << "\n" << timestamp() << "." << microseconds();
//         oss << "\n\n***** FATAL SIGNAL RECEIVED ******* " << std::endl;
//         oss << '"' << message() << '"';
//         return oss.str();
//      }
//
//      // Not crash scenario but LOG or CONTRACT
//      auto level_value = _level.value;
//      if (FATAL.value == level_value) {
//         oss << "\n\t*******\tEXIT trigger caused by LOG(FATAL) entry: \n\t";
//         oss << '"' << message() << '"';
//         oss << "\n*******\tSTACKDUMP *******\n" << internal::stackdump();
//      } else if (internal::CONTRACT.value == level_value) {
//         oss << "\n\t  *******\tEXIT trigger caused by broken Contract: CHECK(" << _expression << ")\n\t";
//         oss << '"' << message() << '"';
//         oss << "\n*******\tSTACKDUMP *******\n" << internal::stackdump() << '"';
//      } else {
//         oss << "\n\t*******\tUNKNOWN Log Message Type\n" << '"' << message() << '"';
//      }
//
//      return oss.str();
//   }

     std::string LogMessage::timestamp(const std::string & time_look) const {
        return  localtime_formatted(_timestamp, time_look);
   }



   std::string LogMessage::toString()  {
      std::string out;
      out.append("\n").append(timestamp()).append(".")
      .append(microseconds()).append("\t")
      .append(level()).append(" [").append(file())
      .append(" L: ").append(line()).append("]\t");
      
      // Non-fatal Log Message
      if (false == wasFatal()) {
         out.append("\"").append(message()).append("\"");
         return out;
      }

      if (internal::FATAL_SIGNAL.value == _level.value) {
         out = {"\n"};
         out.append(timestamp()).append(".").append(microseconds())
         .append("\n\n***** FATAL SIGNAL RECEIVED ******* \n""\"").append(message()).append("\"");
         return out;
      }

      // Not crash scenario but LOG or CONTRACT
      auto level_value = _level.value;
      if (FATAL.value == level_value) {
         out.append("\n\t*******\tEXIT trigger caused by LOG(FATAL) entry: \n\t")
         .append("\"").append(message()).append("\"")
         .append("\n*******\tSTACKDUMP *******\n")
         .append(internal::stackdump());
      } else if (internal::CONTRACT.value == level_value) {
         out.append("\n\t  *******\tEXIT trigger caused by broken Contract: CHECK(").append(_expression)
         .append(")\n\t").append("\"").append(message()).append("\"")
         .append("\n*******\tSTACKDUMP *******\n").append(internal::stackdump()).append("\"");
      } else {
         out.append("\n\t*******\tUNKNOWN Log Message Type\n").append("\"").append(message()).append("\"");
      }

      return out;
   } 
   LogMessage::LogMessage(const std::string &file, const int line,
           const std::string& function, const LEVELS& level)
   : _timestamp(g2::systemtime_now())
   , _microseconds(microsecondsCounter())
   , _file(splitFileName(file)), _line(line), _function(function), _level(level)
{}


   LogMessage::LogMessage(const std::string& fatalOsSignalCrashMessage)
   : LogMessage({""}, 0, {""}, internal::FATAL_SIGNAL) {
      _message.append(fatalOsSignalCrashMessage);
   }
   
   LogMessage::LogMessage(const LogMessage& other) 
   : _timestamp(other._timestamp)
   , _microseconds(other._microseconds)
   , _file(other._file)
   , _line(other._line)
   , _function(other._function)
   , _level(other._level)
   , _expression(other._expression)
   , _message(other._message)
   {
   }

  
   LogMessage::LogMessage(LogMessage&& other)
   : _timestamp(other._timestamp)
   , _microseconds(other._microseconds)
   , _file(std::move(other._file))
   , _line(other._line)
   , _function(std::move(other._function))
   , _level(other._level)
   , _expression(std::move(other._expression))
   , _message(std::move(other._message)) {
   }


//   LogMessage& LogMessage::operator=(const LogMessage& other) {
//      _timestamp = other._timestamp;
//      _microseconds = other._microseconds;
//      _file = other._file;
//      _line=other._line;
//      _function=other._function;
//      _level.value = other._level.value;
//      _level.text = other._level.text;
//      _expression= other._expression;
//      stream().str(other.stream().str());
//   }
  
   
//   LogMessage& LogMessage::operator=(LogMessage&& other) {
//      _timestamp = other._timestamp;
//      _microseconds = other._microseconds;
//      _file = std::move(other._file);
//      _line = other._line;
//      _function = std::move(other._function);
//      _level = other._level;
//      _expression = std::move(other._expression);
//      std::move(_stream, other._stream);
//      return *this;
//   }
//
//      
//     
      

   FatalMessage::FatalMessage(const LogMessage& details, int signal_id) 
   : LogMessage(details), _signal_id(signal_id) { }

   
   
   FatalMessage::FatalMessage(const FatalMessage& other) 
   : LogMessage(other), _signal_id(other._signal_id) {}

      
   LogMessage  FatalMessage::copyToLogMessage() const {
      return LogMessage(*this);
   }
   
   std::string FatalMessage::signal() const{
      return internal::signalName(_signal_id);
   }

   
} // g2
