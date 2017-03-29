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
#include <map>
#include <atomic>
#include <g3log/atomicbool.hpp>

// Levels for logging, made so that it would be easy to change, remove, add levels -- KjellKod
struct LEVELS {
   // force internal copy of the const char*. This is a simple safeguard for when g3log is used in a
   // "dynamic, runtime loading of shared libraries"

   LEVELS(const LEVELS& other): value(other.value), text(other.text.c_str()) {}
   LEVELS(int id, const std::string& idtext) : value(id), text(idtext) {}

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

// If you want to add any extra logging level then please add to your own source file the logging level you need
// then insert it using g3::setLogLevel(...). Please note that this only works for dynamic logging levels.
//
// There should be NO reason for modifying this source file when adding custom levels
//
// When dynamic loggins levels are disabled then adding your own logging levels is not required as
// the new logging level by default will always be enabled.
//
// example: MyLoggingLevel.h
// #pragma once
//  const LEVELS MYINFO {WARNING.value +1, "MyInfoLevel"};
//  const LEVELS MYFATAL {FATAL.value +1, "MyFatalLevel"};
//
//  ... somewhere else when G3_DYNAMIC_LOGGING is enabled
//  setLogLevel(MYINFO, true);
//  LOG(MYINFO) << "some text";
//
//  ... another example, when G3_DYNAMIC_LOGGING is enabled
//  'setLogLevel' is NOT required
//  LOG(MYFATL) << "this will just work, and it will be counted as a FATAL event";
namespace g3 {
   static const int kDebugValue = 100;
   static const int kInfoValue = 300;
   static const int kWarningValue = 500;
   static const int kFatalValue = 1000;
   static const int kInternalFatalValue = 2000;
} // g3


#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
const LEVELS DBUG {g3::kDebugValue, {"DEBUG"}},
#else
const LEVELS DEBUG {g3::kDebugValue, {"DEBUG"}},
#endif
      INFO {g3::kInfoValue, {"INFO"}},
      WARNING {g3::kWarningValue, {"WARNING"}},
      FATAL {g3::kFatalValue, {"FATAL"}};



namespace g3 {
   // Logging level and atomic status collection struct
   struct LoggingLevel {
      atomicbool status;
      LEVELS level;

      // default operator needed for std::map compliance
      LoggingLevel(): status(false), level(INFO) {};
      LoggingLevel(const LoggingLevel& lvl) : status(lvl.status), level(lvl.level) {}
      LoggingLevel(const LEVELS& lvl): status(true), level(lvl) {};
      LoggingLevel(const LEVELS& lvl, bool enabled): status(enabled), level(lvl) {};
      ~LoggingLevel() = default;

      LoggingLevel& operator=(const LoggingLevel& other) {
         status = other.status;
         level = other.level;
         return *this;
      }

      bool operator==(const LoggingLevel& rhs)  const {
         return (status == rhs.status && level == rhs.level);
      }

   };
} // g3




namespace g3 {
   namespace internal {
      const LEVELS CONTRACT {g3::kInternalFatalValue, {"CONTRACT"}},
            FATAL_SIGNAL {g3::kInternalFatalValue + 1, {"FATAL_SIGNAL"}},
            FATAL_EXCEPTION {kInternalFatalValue + 2, {"FATAL_EXCEPTION"}};

      /// helper function to tell the logger if a log message was fatal. If it is it will force
      /// a shutdown after all log entries are saved to the sinks
      bool wasFatal(const LEVELS& level);
   }

#ifdef G3_DYNAMIC_LOGGING
   // Only safe if done at initialization in a single-thread context
   namespace only_change_at_initialization {

      // This sets the logging level status enabled or disabled for a logging level.
      // If the logging level is NEW it will also ADD the logging level to the set of logging levels.
      void setLogLevel(LEVELS level, bool enabled_status);

      /// Enable log level >= log_level
      void setLogLevel(LEVELS level);

      /// print all levels with their disabled or enabled status
      std::string printLevels(std::map<int, g3::LoggingLevel> levelsToPrint);

      /// print snapshot of system levels with their
      /// disabled or enabled status
      std::string printLevels();

      /// reset all default logging levels to enabled
      /// remove any added logging levels so that the only ones left are
      ///  {DEBUG,INFO,WARNING,ERROR,FATAL}
      void reset();

      /// disable all logging levels
      /// WARNING: This will also disable FATAL events from being logged
      void disableAll();

      /// Snapshot view of the current logging levels' status
      std::map<int, g3::LoggingLevel> getAllLevels();
   } // only_change_at_initialization

#endif
   /// Enabled status for the given logging level
   bool logLevel(LEVELS level);

} // g3

