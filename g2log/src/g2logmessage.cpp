/** ==========================================================================
 * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
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
   std::chrono::steady_clock::time_point g_start_time;

   int64_t  microsecondsCounter() {
      std::call_once(g_start_time_flag, []() { g_start_time = std::chrono::steady_clock::now(); });
      auto  now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::microseconds>(now - g_start_time).count();
   }

   std::string splitFileName(const std::string& str) {
      size_t found;
      found = str.find_last_of("(/\\");
      return str.substr(found + 1);
   }
} // anonymous



namespace g2 {

   std::string LogMessage::toString() const {
      std::string out;
      out.append("\n" + timestamp() + "." + microseconds() +  "\t"
              + level() + " [" + file() + " L: " + line() + "]\t");

      // Non-fatal Log Message
      if (false == wasFatal()) {
         out.append('"' + message() + '"');
         return out;
      }

      if (internal::FATAL_SIGNAL.value == _level.value) {
         out.clear(); // clear any previous text and formatting
         out.append("\n" + timestamp() + "." + microseconds() 
                 + "\n\n***** FATAL SIGNAL RECEIVED ******* \n"
                 + '"' + message() + '"');
         return out;
      }

      // Not crash scenario but LOG or CONTRACT
      auto level_value = _level.value;
      if (FATAL.value == level_value) {
         static const std::string fatalExitReason = {"EXIT trigger caused by LOG(FATAL) entry: "};
         out.append("\n\t*******\t " + fatalExitReason + "\n\t" + '"' + message() + '"');
      } else if (internal::CONTRACT.value == level_value) {
         static const std::string contractExitReason = {"EXIT trigger caused by broken Contract:"};
         out.append("\n\t*******\t " + contractExitReason + " CHECK(" + _expression + ")\n\t" 
                 + '"' + message() + '"');
      } else {
         static const std::string errorUnknown = {"UNKNOWN Log Message Type"};
         out.append("\n\t*******" + errorUnknown + "\t\n" + '"' + message() + '"');
      }
 
      return out;
   }

     std::string LogMessage::timestamp(const std::string & time_look) const {
        return  localtime_formatted(_timestamp, time_look);
   }

   LogMessage::LogMessage(const std::string &file, const int line,
           const std::string& function, const LEVELS& level)
   : _timestamp(g2::systemtime_now())
   , _call_thread_id(std::this_thread::get_id())
   , _microseconds(microsecondsCounter())
   , _file(splitFileName(file))
   , _line(line)
   , _function(function)
   , _level(level)
{}


   LogMessage::LogMessage(const std::string& fatalOsSignalCrashMessage)
   : LogMessage({""}, 0, {""}, internal::FATAL_SIGNAL) {
      _message.append(fatalOsSignalCrashMessage);
   }
   
   LogMessage::LogMessage(const LogMessage& other) 
   : _timestamp(other._timestamp)
   , _call_thread_id(other._call_thread_id)
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
   , _call_thread_id(other._call_thread_id)
   , _microseconds(other._microseconds)
   , _file(std::move(other._file))
   , _line(other._line)
   , _function(std::move(other._function))
   , _level(other._level)
   , _expression(std::move(other._expression))
   , _message(std::move(other._message)) {
   }    
   
   
   std::string LogMessage::threadID() const {
      std::ostringstream oss;
      oss << _call_thread_id;
      return oss.str();
   }

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
