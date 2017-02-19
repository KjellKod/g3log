#pragma once
/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================
* Filename:g3time.h cross-platform, thread-safe replacement for C++11 non-thread-safe
*                   localtime (and similar)
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not under copywrite protection. First published for g3log at KjellKod.cc
* ********************************************* */

#include <ctime>
#include <string>
#include <chrono>

// FYI:
// namespace g3::internal ONLY in g3time.cpp
//          std::string put_time(const struct tm* tmb, const char* c_time_format)

namespace g3 {
   namespace internal {
      enum class Fractional {Millisecond, Microsecond, Nanosecond, NanosecondDefault};
      Fractional getFractional(const std::string& format_buffer, size_t pos);
      std::string to_string(const timespec& time_snapshot, Fractional fractional);
      static const std::string date_formatted = "%Y/%m/%d";
      // %f: fractions of seconds (%f is nanoseconds)
      // %f3: milliseconds, 3 digits: 001
      // %6: microseconds: 6 digits: 000001  --- default for the time_format
      // %f9, %f: nanoseconds, 9 digits: 000000001
      static const std::string time_formatted = "%H:%M:%S %f6";
   } // internal

   typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;
   typedef std::chrono::milliseconds milliseconds;
   typedef std::chrono::microseconds microseconds;

      

   //  custom wrap for std::chrono::system_clock::now()but this one 
   // returns timespec struct instead which is what we use in g3log
   struct timespec systemtime_now();

   // OSX, Windows needed wrapper for std::timespec_get(struct timespec *ts, int base)
   //   OSX and Windows also lacks the POSIX clock_gettime(int base, struct timespec *ts)
   //   so for that reason we go with the std::timespec_get(...) but wrap it
   int timespec_get(struct timespec* ts/*, int base*/);

   // This mimics the original "std::put_time(const std::tm* tmb, const charT* fmt)"
   // This is needed since latest version (at time of writing) of gcc4.7 does not implement this library function yet.
   // return value is SIMPLIFIED to only return a std::string
   std::string put_time(const struct tm* tmb, const char* c_time_format);

   /** return time representing POD struct (ref ctime + wchar) that is normally
   * retrieved with std::localtime. g3::localtime is threadsafe which std::localtime is not.
   * g3::localtime is probably used together with @ref g3::systemtime_now */
   tm localtime(const std::time_t& time);

   /** format string must conform to std::put_time's demands.
   * WARNING: At time of writing there is only so-so compiler support for
   * std::put_time. A possible fix if your c++11 library is not updated is to
   * modify this to use std::strftime instead */
   std::string localtime_formatted(const timespec& time_snapshot, const std::string& time_format) ;
}



