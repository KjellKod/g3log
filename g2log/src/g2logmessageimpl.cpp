

#include <g2logmessageimpl.hpp>
#include <g2time.hpp>
#include <mutex>

namespace {
   const int kMaxMessageSize = 2048;
   const std::string kTruncatedWarningText = "[...truncated...]";
   std::once_flag g_start_time_flag;
   g2::steady_time_point g_start_time;

   long microsecondsCounter() {
      std::call_once(g_start_time_flag, []() { g_start_time =  std::chrono::steady_clock::now();  });
      g2::steady_time_point  now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::microseconds>(g_start_time - now).count();
   }

   std::string splitFileName(const std::string& str) {
      size_t found;
      found = str.find_last_of("(/\\");
      return str.substr(found + 1);
   }
} // anonymous


namespace g2 {
   LogMessageImpl::LogMessageImpl(const std::string &file, const int line,
           const std::string& function, const LEVELS& level)
   : _timestamp(g2::systemtime_now())
   , _microseconds(microsecondsCounter())
   , _file(splitFileName(file)), _line(line), _function(function), _level(level) { }


   LogMessageImpl::LogMessageImpl(const std::string& fatalOsSignalCrashMessage)
   : LogMessageImpl({""}, 0, {""}, internal::FATAL_SIGNAL) {
      _stream << fatalOsSignalCrashMessage;
   }
} // g2