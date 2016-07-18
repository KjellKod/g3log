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


namespace {
   const size_t kMsDigitsToCut = 1000000; // 3 digits left of 1000 000 000
   const size_t kUsDigitsToCut = 1000; // 6 digits left
   const size_t kNsDigitsToCut = 1; // all digits left : divide by 1
   const size_t kNsDefaultDigits = 0;
   const size_t kDigitIdentifier = 1;
   const size_t kNoDigitIdentifier = 0;

   const std::string kIdentifier   = "%f";
   const size_t kIdentifierSize = 2;


   std::string fractional_to_string(size_t fractional, size_t cutOffDivider) {
      auto zeroes = 9; // default ns
      if (kMsDigitsToCut ==  cutOffDivider) {
         zeroes = 3;
      } else if (kUsDigitsToCut == cutOffDivider) {
         zeroes = 6;
      } else {
         cutOffDivider = 1;
      }
      fractional /= cutOffDivider;

      // auto value = std::to_string(typeAdjustedValue);
      // return value; // std::string(fractional_digit, '0') + value;
      auto value = std::string(std::to_string(fractional));
      return std::string(zeroes - value.size(), '0') + value;
   }
} // anonymous


namespace g3 {
   namespace internal {
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
         // TODO: incorrect code
         if (0 == success) {
            assert((0 != success) && "strftime fails with illegal formatting");
            return c_time_format;
         }

         return buffer;
#endif
      }
   } // internal
} // g3



namespace g3 {

   std::time_t systemtime_now() {
      system_time_point system_now = std::chrono::system_clock::now();
      return std::chrono::system_clock::to_time_t(system_now);
   }


   int timespec_get(struct timespec* ts/*, int base*/) {
#ifdef __MACH__
      // std::timespec_get or posix clock_gettime)(...) are not
      // implemented on OSX
      // @return value of base if successful, else zero
      struct timeval now = {};
      int rv = gettimeofday(&now, nullptr);
      if (-1 == rv) {
         return rv;
      }
      // error mode. just return sec, microsecond
      ts->tv_sec  = now.tv_sec;
      ts->tv_nsec = now.tv_usec * 1000;
      return 0;
#else
      // ubuntu/gcc5 has no support for std::timespec_get(...) as of yet
      return clock_gettime(CLOCK_REALTIME, ts);
#endif
   }






   tm localtime(const std::time_t& time) {
      struct tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) && !defined(__GNUC__))
      localtime_s(&tm_snapshot, &time); // windsows
#else
      localtime_r(&time, &tm_snapshot); // POSIX
#endif
      return tm_snapshot;
   }



   size_t fractional_cutoff(const std::string& format_buffer, size_t pos) {
      char  ch  = (format_buffer.size() > pos + kIdentifierSize ? format_buffer.at(pos + kIdentifierSize) : '\0');
      size_t cutOff = 0;
      switch (ch) {
         case '3': cutOff = kMsDigitsToCut; break;
         case '6': cutOff = kUsDigitsToCut; break;
         case '9': cutOff = kNsDigitsToCut; break;
         default: cutOff = kNsDefaultDigits; break;
      }
      return cutOff;
   }


   std::string localtime_formatted(const timespec& time_snapshot, const std::string& time_format) {
      auto format_buffer = time_format;  // copying format string to a separate buffer

      // iterating through every "%f" instance in the format string
      auto identifierExtraSize = 0;
      for (size_t pos = 0; (pos = format_buffer.find(kIdentifier, pos)) != std::string::npos; pos += kIdentifierSize + identifierExtraSize) {
         // figuring out whether this is nano, micro or milli identifier
         auto cutoff = fractional_cutoff(format_buffer, pos);
         auto value = fractional_to_string(time_snapshot.tv_nsec, cutoff);
         auto identifierExtraSize = cutoff == kNsDefaultDigits ? kNoDigitIdentifier : kDigitIdentifier;

         // replacing "%f[3|6|9]" with sec fractional part value
         format_buffer.replace(pos, kIdentifier.size() + identifierExtraSize, value);
      }

      return localtime_formatted(time_snapshot.tv_sec, format_buffer);
   }

   std::string localtime_formatted(const std::time_t& time_snapshot, const std::string& time_format) {
      std::tm t = localtime(time_snapshot); // could be const, but cannot due to VS2012 is non conformant for C++11's std::put_time (see above)
      return g3::internal::put_time(&t, time_format.c_str()); // format example: //"%Y/%m/%d %H:%M:%S");
   }
} // g3
