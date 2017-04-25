/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/time.hpp"

#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <chrono>
#include <cassert>
#include <iomanip>
#ifdef __MACH__
#include <sys/time.h>
#endif

namespace g3 {
   namespace internal {
      const std::string kFractionalIdentier   = "%f";
      const size_t kFractionalIdentierSize = 2;



      Fractional getFractional(const std::string& format_buffer, size_t pos) {
         char  ch  = (format_buffer.size() > pos + kFractionalIdentierSize ? format_buffer.at(pos + kFractionalIdentierSize) : '\0');
         Fractional type = Fractional::NanosecondDefault;
         switch (ch) {
            case '3': type = Fractional::Millisecond; break;
            case '6': type = Fractional::Microsecond; break;
            case '9': type = Fractional::Nanosecond; break;
            default: type = Fractional::NanosecondDefault; break;
         }
         return type;
      }


/*
   localtime_formatted_fractions(const system_time_point& ts, const std::string& time_format);
      // iterating through every "%f" instance in the format string
      auto identifierExtraSize = 0;
      for (size_t pos = 0; 
         (pos = format_buffer.find(g3::internal::kFractionalIdentier, pos)) != std::string::npos; 
         pos += g3::internal::kFractionalIdentierSize + identifierExtraSize) {
         // figuring out whether this is nano, micro or milli identifier
         auto type = g3::internal::getFractional(format_buffer, pos);
         auto value = g3::internal::to_string(ts, type);
         auto padding = 0;
         if (type != g3::internal::Fractional::NanosecondDefault) {
            padding = 1;
         }

         // replacing "%f[3|6|9]" with sec fractional part value
         format_buffer.replace(pos, g3::internal::kFractionalIdentier.size() + padding, value);
      }


*/
      // Returns the fractional as a string with padded zeroes
      // %f: fractions of seconds (%f is nanoseconds)
      // %f3: ms -> milliseconds, 3 digits: 001
      // %6: us -> microseconds: 6 digits: 000001  --- default for the time_format
      // %f9, %f: ns -> nanoseconds, 9 digits: 000000001
      std::string localtime_formatted_fractions(const g3::system_time_point& ts, std::string format_buffer) {
         auto duration = ts.time_since_epoch();
         
         auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration);
         duration -= sec;
         auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
         duration -= ms;
         auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration);
         duration -= us;
         auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

         .... wrong. The formatting is ONLY for nanoseconds. 
         I.e. we must follow the old approach of only retrieving the nanoseconds
         and then use division to get the value.

         
         
         // iterating through every "%f" instance in the format string
         auto identifierExtraSize = 0;
         for (size_t pos = 0; 
            (pos = format_buffer.find(g3::internal::kFractionalIdentier, pos)) != std::string::npos; 
            pos += g3::internal::kFractionalIdentierSize + identifierExtraSize) {
            // figuring out whether this is nano, micro or milli identifier
            auto type = g3::internal::getFractional(format_buffer, pos);
            //auto digits = static_cast<size_t>(type);


            auto number = 0;
            auto zeroes = 9; // default ns
            switch (type) {
               case Fractional::Millisecond : {
                  zeroes = 3;
                  number = ms.count();
                  break;
               }
               case Fractional::Microsecond : {
                  zeroes = 6;
                  number = us.count();
                  break;
               }
               case Fractional::Nanosecond :
               case Fractional::NanosecondDefault:
               default:
                  zeroes = 9;
                  number = ns.count();
            }

            auto padding = 0;
            if (type != g3::internal::Fractional::NanosecondDefault) {
               padding = 1;
            }
         
            auto value = std::to_string(number);
            return std::string(zeroes - value.size(), '0') + value;

            // replacing "%f[3|6|9]" with sec fractional part value
            format_buffer.replace(pos, g3::internal::kFractionalIdentier.size() + padding, value);
         }
         return format_buffer;
      }
   } // internal
} // g3



namespace g3 {
//   struct timespec systemtime_now() {
//      struct timespec ts = {};
//      timespec_get(&ts);
//      return ts;
//   }


   // std::timespec_get or posix clock_gettime)(...) are not
   // implemented on OSX and ubuntu gcc5 has no support for std::timespec_get(...) as of yet
   // so instead we roll our own.
   //int timespec_get(struct timespec* ts/*, int base*/) {
   //   using namespace std::chrono;
      
      // thanks @AndreasSchoenle for the implementation and the explanation:
      // The time since epoch for the steady_clock is not necessarily really the time since 1970.
      // It usually is the time since program start. Thus, here is calculated the offset between 
      // the starting point and the real start of the epoch as reported by the system clock 
      // with the precision of the system clock. 
      // 
      // Time stamps will later have system clock accuracy but relative times will have the precision
      // of the high resolution clock.   
      //thread_local const auto os_system =
      //   time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch();
      //thread_local const auto os_high_resolution = 
      //   time_point_cast<nanoseconds>(high_resolution_clock::now()).time_since_epoch();
      //thread_local auto os = os_system - os_high_resolution;

      // 32-bit system work-around, where apparenetly the os correction above could sometimes 
      // become negative. This correction will only be done once per thread
      //if (os.count() < 0 ) {
      //   os =  os_system;
      //}

      //auto now_ns = (time_point_cast<nanoseconds>(high_resolution_clock::now()).time_since_epoch() + os).count();
      //const auto kNanos = 1000000000;
      //ts ->tv_sec = now_ns / kNanos;
      //ts ->tv_nsec = now_ns % kNanos;

      //const auto kNanos = 1000000000;
     // auto now = system_clock::now();
     // ts ->tv_sec = time_point_cast<seconds>(now).time_since_epoch().count();
     // ts ->tv_nsec = time_point_cast<nanoseconds>(now).time_since_epoch().count();
     // #ifdef TIME_UTC
     //    return TIME_UTC;
     // #endif
     // return 1;
   //}



   // This mimics the original "std::put_time(const std::tm* tmb, const charT* fmt)"
   // This is needed since latest version (at time of writing) of gcc4.7 does not implement this library function yet.
   // return value is SIMPLIFIED to only return a std::string
   std::string put_time(const struct tm* tmb, const char* c_time_format) {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
      std::ostringstream oss;
      oss.fill('0');
      // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
      oss << std::put_time(const_cast<struct tm*> (tmb), c_time_format);
      return oss.str();
#else    // LINUX
      const size_t size = 1024;
      char buffer[size]; // IMPORTANT: check now and then for when gcc will implement std::put_time.
      //                    ... also ... This is way more buffer space then we need

     auto success = std::strftime(buffer, size, c_time_format, tmb);
      // In DEBUG the assert will trigger a process exit. Once inside the if-statement
      // the 'always true' expression will be displayed as reason for the exit
      //
      // In Production mode
      // the assert will do nothing but the format string will instead be returned
      if (0 == success) {
         assert((0 != success) && "strftime fails with illegal formatting");
         return c_time_format;
      }

      return buffer;
#endif
   }



   tm localtime(const std::time_t& ts) {
      struct tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) && !defined(__GNUC__))
      localtime_s(&tm_snapshot, &ts); // windsows
#else
      localtime_r(&ts, &tm_snapshot); // POSIX
#endif
      return tm_snapshot;
   }




   std::string localtime_formatted(const g3::system_time_point& ts, const std::string& time_format) {
      auto format_buffer = internal::localtime_formatted_fractions(ts, time_format);
      auto time_point = std::chrono::system_clock::to_time_t(ts);
      std::tm t = localtime(time_point);   
      return g3::put_time(&t, format_buffer.c_str()); // format example: //"%Y/%m/%d %H:%M:%S");
   }
} // g3
