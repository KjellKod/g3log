/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#pragma once
#include "g3log/generated_definitions.hpp"

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
#include <algorithm>


// Levels for logging, made so that it would be easy to change, remove, add levels -- KjellKod
struct LEVELS {
   // force internal copy of the const char*. This is a simple safeguard for when g3log is used in a
   // "dynamic, runtime loading of shared libraries"

   LEVELS(const LEVELS& other): value(other.value), text(other.text.c_str()) {}
   LEVELS(int id, const char* idtext) : value(id), text(idtext) {}

   bool operator==(const LEVELS& rhs)  const {
      return (value == rhs.value && text == rhs.text);
   }

   bool operator!=(const LEVELS& rhs) const {
      return (value != rhs.value || text != rhs.text);
   }

   friend void swap(LEVELS& first, LEVELS& second) {
      using std::swap;
      swap(first.value, second.value);
      swap(first.text, second.text);
   }


   LEVELS& operator=(LEVELS other) {
      swap(*this, other);
      return *this;
   }


   int value;
   std::string text;
};



namespace g3 {
   static const int kDebugVaulue = 0;
   static const int kInfoValue = 100;
   static const int kWarningValue = 500;
   static const int kFatalValue = 1000;
   static const int kInternalFatalValue = 2000;
}

#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
const LEVELS DBUG {g3::kDebugVaulue, {"DEBUG"}},
#else
const LEVELS DEBUG {g3::kDebugVaulue, {"DEBUG"}},
#endif
      INFO {g3::kInfoValue, {"INFO"}},
      WARNING {g3::kWarningValue, {"WARNING"}},



// Insert here *any* extra logging levels that is needed. You can do so in your own source file
// If it is a FATAL you should keep it above (FATAL.value and below internal::CONTRACT.value
// If it is a non-fatal you can keep it above (WARNING.value and below FATAL.value)
//
// example: MyLoggingLevel.h
// #pragma once
//  const LEVELS MYINFO {WARNING.value +1, {"MyInfoLevel"}};
//  const LEVELS MYFATAL {FATAL.value +1, {"MyFatalLevel"}};
//
// IMPORTANT: As of yet dynamic on/off of logging is NOT changed automatically
//     any changes of this, if you use dynamic on/off must be done in loglevels.cpp,
//     g_log_level_status and
//     void setLogLevel(LEVELS log_level, bool enabled) {...}
//     bool logLevel(LEVELS log_level){...}


// 1) Remember to update the FATAL initialization below
// 2) Remember to update the initialization of "g3loglevels.cpp/g_log_level_status"
      FATAL {g3::kFatalValue, {"FATAL"}};

namespace g3 {
   namespace internal {
      const LEVELS CONTRACT {g3::kInternalFatalValue, {"CONTRACT"}},
            FATAL_SIGNAL {g3::kInternalFatalValue +1, {"FATAL_SIGNAL"}},
            FATAL_EXCEPTION {kInternalFatalValue +2, {"FATAL_EXCEPTION"}};

      /// helper function to tell the logger if a log message was fatal. If it is it will force
      /// a shutdown after all log entries are saved to the sinks
      bool wasFatal(const LEVELS& level);
   }

#ifdef G3_DYNAMIC_LOGGING
   // Only safe if done at initialization in a single-thread context
   namespace only_change_at_initialization {
      // Enable/Disable a log level {DEBUG,INFO,WARNING,FATAL}
      void setLogLevel(LEVELS level, bool enabled_status);
      std::string printLevels();
      void reset();

   } // only_change_at_initialization
#endif
   bool logLevel(LEVELS level);
} // g3

