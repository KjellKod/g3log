#ifndef G2_TIME_H_
#define G2_TIME_H_
/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2time.h cross-platform, thread-safe replacement for C++11 non-thread-safe
*                   localtime (and similar)
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not under copywrite protection. First published for g2log at KjellKod.cc
* ********************************************* */

#include <ctime>
#include <string>
#include <chrono>

// FYI: 
// namespace g2::internal ONLY in g2time.cpp
//          std::string put_time(const struct tm* tmb, const char* c_time_format)

namespace g2
{
  typedef std::chrono::steady_clock::time_point steady_time_point;
  typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;
  typedef std::chrono::milliseconds milliseconds;
  typedef std::chrono::microseconds microseconds;

  //  wrap for std::chrono::system_clock::now()
  std::time_t systemtime_now();

  /** return time representing POD struct (ref ctime + wchar) that is normally
  * retrieved with std::localtime. g2::localtime is threadsafe which std::localtime is not.
  * g2::localtime is probably used together with @ref g2::systemtime_now */
  tm localtime(const std::time_t& time);

  /** format string must conform to std::put_time's demands.
  * WARNING: At time of writing there is only so-so compiler support for
  * std::put_time. A possible fix if your c++11 library is not updated is to
  * modify this to use std::strftime instead */
  std::string localtime_formatted(const std::time_t& time_snapshot, const std::string& time_format) ;
}

#endif
