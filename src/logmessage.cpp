/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/logmessage.hpp"
#include "g3log/crashhandler.hpp"
#include "g3log/time.hpp"
#include <mutex>
#include <locale>
#if !defined(__GNUC__) || (__GNUC__ >= 5)
#include <codecvt>
#else
// Unfortunately no standard way to convert from wchar_t to Utf-8 narrow string. Sinks that do not use the
// message() function can implement their own. 
#endif

namespace {
   std::string splitFileName(const std::string& str) {
      size_t found;
      found = str.find_last_of("(/\\");
      return str.substr(found + 1);
   }
} // anonymous



namespace g3 {


   // helper for setting the normal log details in an entry
   std::string LogDetailsToString(const LogMessage& msg) {
      std::string out;
      out.append("\n" + msg.timestamp() + "\t"
                 + msg.level() + " [" + msg.file() + "->" + msg.function() + ":" + msg.line() + "]\t");
      return out;
   }


   // helper for normal
   std::string normalToString(const LogMessage& msg) {
      auto out = LogDetailsToString(msg);
      out.append('"' + msg.message() + '"');
      return out;
   }

   // helper for fatal signal
   std::string  fatalSignalToString(const LogMessage& msg) {
      std::string out; // clear any previous text and formatting
      out.append("\n" + msg.timestamp()
                 + "\n\n***** FATAL SIGNAL RECEIVED ******* \n"
                 + '"' + msg.message() + '"');
      return out;
   }


   // helper for fatal exception (windows only)
   std::string  fatalExceptionToString(const LogMessage& msg) {
      std::string out; // clear any previous text and formatting
      out.append("\n" + msg.timestamp()
                 + "\n\n***** FATAL EXCEPTION RECEIVED ******* \n"
                 + '"' + msg.message() + '"');
      return out;
   }


   // helper for fatal LOG
   std::string fatalLogToString(const LogMessage& msg) {
      auto out = LogDetailsToString(msg);
      static const std::string fatalExitReason = {"EXIT trigger caused by LOG(FATAL) entry: "};
      out.append("\n\t*******\t " + fatalExitReason + "\n\t" + '"' + msg.message() + '"');
      return out;
   }

   // helper for fatal CHECK
   std::string fatalCheckToString(const LogMessage& msg) {
      auto out = LogDetailsToString(msg);
      static const std::string contractExitReason = {"EXIT trigger caused by broken Contract:"};
      out.append("\n\t*******\t " + contractExitReason + " CHECK(" + msg.expression() + ")\n\t"
                 + '"' + msg. message() + '"');
      return out;
   }


   // Format the log message according to it's type
   std::string LogMessage::toString() const {
      if (false == wasFatal()) {
         return normalToString(*this);
      }

      const auto level_value = _level.value;
      if (internal::FATAL_SIGNAL.value == _level.value) {
         return fatalSignalToString(*this);
      }

      if (internal::FATAL_EXCEPTION.value == _level.value) {
         return fatalExceptionToString(*this);
      }

      if (FATAL.value == _level.value) {
         return fatalLogToString(*this);
      }

      if (internal::CONTRACT.value == level_value) {
         return fatalCheckToString(*this);
      }

      // What? Did we hit a custom made level?
      auto out = LogDetailsToString(*this);
      static const std::string errorUnknown = {"UNKNOWN or Custom made Log Message Type"};
      out.append("\n\t*******" + errorUnknown + "\t\n" + '"' + message() + '"');
      return out;
   }

   std::string LogMessage::message() const {
      std::string msg;
      if (!_wmessage.empty())
      {
#if !defined(__GNUC__) || (__GNUC__ >= 5)
         std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
         msg.append(_message.append(convert.to_bytes(_wmessage)));
#else
         msg.append("[unconverted wstring]");
#endif
      }
      return msg.append(_message);
   }

   std::string LogMessage::timestamp(const std::string& time_look) const {
      return g3::localtime_formatted(_timestamp, time_look);
   }


// By copy, not by reference. See this explanation for details:
// http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
   LogMessage& LogMessage::operator=(LogMessage other) {
      swap(*this, other);
      return *this;
   }


   LogMessage::LogMessage(const std::string& file, const int line,
                          const std::string& function, const LEVELS& level)
      : _call_thread_id(std::this_thread::get_id())
      , _file(splitFileName(file))
      , _file_path(file)
      , _line(line)
      , _function(function)
      , _level(level)
   {
      g3::timespec_get(&_timestamp/*, TIME_UTC*/);
      // Another possibility could be to Falling back to clock_gettime as TIME_UTC 
      // is not recognized by travis CI. 
      // i.e. clock_gettime(CLOCK_REALTIME, &_timestamp);
   }


   LogMessage::LogMessage(const std::string& fatalOsSignalCrashMessage)
      : LogMessage( {""}, 0, {""}, internal::FATAL_SIGNAL) {
      _message.append(fatalOsSignalCrashMessage);
   }

   LogMessage::LogMessage(const LogMessage& other)
      : _timestamp(other._timestamp)
      , _call_thread_id(other._call_thread_id)
      , _file(other._file)
      , _file_path(other._file_path)
      , _line(other._line)
      , _function(other._function)
      , _level(other._level)
      , _expression(other._expression)
      , _message(other._message) 
	  , _wmessage(other._wmessage) {
   }

   LogMessage::LogMessage(LogMessage &&other)
      : _timestamp(other._timestamp)
      , _call_thread_id(other._call_thread_id)
      , _file(std::move(other._file))
      , _file_path(std::move(other._file_path))
      , _line(other._line)
      , _function(std::move(other._function))
      , _level(other._level)
      , _expression(std::move(other._expression))
      , _message(std::move(other._message))
      , _wmessage(std::move(other._wmessage)) {
   }

   std::string LogMessage::threadID() const {
      std::ostringstream oss;
      oss << _call_thread_id;
      return oss.str();
   }



   FatalMessage::FatalMessage(const LogMessage& details, g3::SignalType signal_id)
      : LogMessage(details), _signal_id(signal_id) { }



   FatalMessage::FatalMessage(const FatalMessage& other)
      : LogMessage(other), _signal_id(other._signal_id) {}


   LogMessage  FatalMessage::copyToLogMessage() const {
      return LogMessage(*this);
   }

   std::string FatalMessage::reason() const {
      return internal::exitReasonName(_level, _signal_id);
   }


} // g3
