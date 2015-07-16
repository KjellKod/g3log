/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#pragma once


// Users of Juce or other libraries might have a define DEBUG which clashes with
// the DEBUG logging level for G3log. In that case they can instead use the define
//  "CHANGE_G3LOG_DEBUG_TO_DBUG" and G3log's logging level DEBUG is changed to be DBUG
#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
#if (defined(DBUG))
#error "DEBUG is already defined elsewhere which clashes with G3Log's log level DEBUG"
#endif
#else
#if (defined(DEBUG))
#error "DEBUG is already defined elsewhere which clashes with G3Log's log level DEBUG"
#endif
#endif

#include <string>


// Levels for logging, made so that it would be easy to change, remove, add levels -- KjellKod
struct LEVELS {
   // force internal copy of the const char*. This is a simple safeguard for when g3log is used in a
   // "dynamic, runtime loading of shared libraries"

   LEVELS(const LEVELS &other)
      : value(other.value), text(other.text.c_str()) {}

   LEVELS(int id, const char *idtext) : value(id), text(idtext) {}

   friend bool operator==(const LEVELS &lhs, const LEVELS &rhs) {
      return (lhs.value == rhs.value && lhs.text == rhs.text);
   }

   const int value;
   const std::string text;
};



namespace g2 {
   static const int kDebugVaulue = 0;
}

#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
const LEVELS DBUG {
   g2::kDebugVaulue, {"DEBUG"}
},
#else
const LEVELS DEBUG {
   g2::kDebugVaulue, {"DEBUG"}
},
#endif
INFO {g2::kDebugVaulue + 1, {"INFO"}},
WARNING {INFO.value + 1, {"WARNING"}},
// Insert here *any* extra logging levels that is needed
// 1) Remember to update the FATAL initialization below
// 2) Remember to update the initialization of "g2loglevels.cpp/g_log_level_status"
FATAL {WARNING.value + 1, {"FATAL"}};


namespace g2 {
   namespace internal {
      const LEVELS CONTRACT {
         100, {"CONTRACT"}
      }, FATAL_SIGNAL {101, {"FATAL_SIGNAL"}},
      FATAL_EXCEPTION {102, {"FATAL_EXCEPTION"}};
      bool wasFatal(const LEVELS &level);
   }

#ifdef G2_DYNAMIC_LOGGING
   // Enable/Disable a log level {DEBUG,INFO,WARNING,FATAL}
   void setLogLevel(LEVELS level, bool enabled_status);
#endif
   bool logLevel(LEVELS level);
} // g2

