/** ==========================================================================
 * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================
 * Filename:g2loglevels.hpp  Part of Framework for Logging and Design By Contract
 * Created: 2012 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#pragma once

#include <string>

// Levels for logging, made so that it would be easy to change, remove, add levels -- KjellKod
struct LEVELS {
   // force internal copy of the const char*. This is a simple safeguard for when g3log is used in a
   // "dynamic, runtime loading of shared libraries"

   LEVELS(const LEVELS& other)
   : value(other.value), text(other.text.c_str()) {}

   LEVELS(int id, const char* idtext) : value(id), text(idtext) {}
   const int value;
   const std::string text;
};

const LEVELS DEBUG{0, {"DEBUG"}}, INFO{DEBUG.value + 1, {"INFO"}},
WARNING{INFO.value + 1, {"WARNING"}},
// Insert here *any* extra logging levels that is needed
// 1) Remember to update the FATAL initialization below
// 2) Remember to update the initialization of "g2loglevels.cpp/g_log_level_status"
FATAL{WARNING.value + 1, {"FATAL"}};


namespace g2 {
   namespace internal {
      const LEVELS CONTRACT{100, {"CONTRACT"}}, FATAL_SIGNAL{101, {"FATAL_SIGNAL"}};
      bool wasFatal(const LEVELS& level);
   }

#ifdef G2_DYNAMIC_LOGGING
   // Enable/Disable a log level {DEBUG,INFO,WARNING,FATAL}
   void setLogLevel(LEVELS level, bool enabled_status);
#endif
   bool logLevel(LEVELS level);
} // g2

