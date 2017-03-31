/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/loglevels.hpp"
#include <cassert>

#include <iostream>

namespace g3 {
   namespace internal {
      bool wasFatal(const LEVELS& level) {
         return level.value >= FATAL.value;
      }

#ifdef G3_DYNAMIC_LOGGING
      const std::map<int, LoggingLevel> g_log_level_defaults = {
         {g3::kDebugValue, {DEBUG}},
         {INFO.value, {INFO}},
         {WARNING.value, {WARNING}},
         {FATAL.value, {FATAL}}
      };

      std::map<int, g3::LoggingLevel> g_log_levels = g_log_level_defaults;
#endif
   } // internal

#ifdef G3_DYNAMIC_LOGGING
   namespace only_change_at_initialization {

      void setLogLevel(LEVELS lvl, bool enabled) {
         int value = lvl.value;
         internal::g_log_levels[value] = {lvl, enabled};
      }


      void addLogLevel(LEVELS level) {
         setLogLevel(level, true);
      }


      void setHighestLogLevel(LEVELS enabledFrom) {
         auto it = internal::g_log_levels.find(enabledFrom.value);
         if (it == internal::g_log_levels.end()) {
            addLogLevel(enabledFrom);
         }
         for (auto& v : internal::g_log_levels) {
            if (v.first < enabledFrom.value) {
               setLogLevel(v.second.level, false);
            }
         }
      }


      std::string printLevels(std::map<int, g3::LoggingLevel> levelsToPrint) {

         std::string levels;
         for (auto& v : levelsToPrint) {
            levels += "name: " + v.second.level.text + " level: " + std::to_string(v.first) + " status: " + std::to_string(v.second.status.value()) + "\n";
         }
         return levels;
      }

      std::string printLevels() {
         return printLevels(internal::g_log_levels);
      }

      void reset() {
         g3::internal::g_log_levels = g3::internal::g_log_level_defaults;
      }

      void disableAll() {
         for (auto& v : internal::g_log_levels) {
            v.second.status = false;
         }
      }

      std::map<int, g3::LoggingLevel> getAllLevels() {
         return internal::g_log_levels;
      }

      //enum class level_status {Absent, Enabled, Disabled};
      level_status LevelStatus(LEVELS level) {
         const auto it = internal::g_log_levels.find(level.value);
         if (internal::g_log_levels.end() == it) {
            return level_status::Absent;
         }

         return (it->second.status.get().load() ? level_status::Enabled : level_status::Disabled);

      }



   } // only_change_at_initialization

#endif


   bool logLevel(LEVELS log_level) {
#ifdef G3_DYNAMIC_LOGGING
      int level = log_level.value;
      bool status = internal::g_log_levels[level].status.value();
      return status;
#endif
      return true;
   }
} // g3
