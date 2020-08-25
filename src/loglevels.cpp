/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/loglevels.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace g3 {

    // LEVELS should be POD.
    static_assert(std::is_standard_layout<LEVELS>::value, "LEVELS must be a standard layout.");
    static_assert(std::is_trivially_copyable<LEVELS>::value, "LEVELS must be a trivially copyable.");
    static_assert(std::is_trivially_default_constructible<LEVELS>::value, "LEVELS must be a trivially default constructible.");
    static_assert(std::is_trivially_destructible<LEVELS>::value, "LEVELS must be a trivially destructible.");

    // LoggingLevel cannot be POD because of atomicbool.
    static_assert(std::is_standard_layout<LoggingLevel>::value, "LoggingLevel must be a standard layout.");
    static_assert(std::is_trivially_destructible<LoggingLevel>::value, "LoggingLevel must be a trivially destructible.");

   namespace internal {
      constexpr size_t kMaxLogLevels = 100;

      bool wasFatal(const LEVELS& level) {
         return level.value >= FATAL.value;
      }

#ifdef G3_DYNAMIC_LOGGING
      const std::array<std::pair<int, LoggingLevel>, 4> g_log_level_defaults = {{
	     {G3LOG_DEBUG.value,{G3LOG_DEBUG}},
         {INFO.value, {INFO}},
         {WARNING.value, {WARNING}},
         {FATAL.value, {FATAL}}
      }};

      std::array<std::pair<int, g3::LoggingLevel>, 100> g_log_levels;
      size_t g_log_levels_size = 0;
#endif
   } // internal

#ifdef G3_DYNAMIC_LOGGING
   namespace only_change_at_initialization {

      void addLogLevel(LEVELS lvl, bool enabled) {
         if (internal::g_log_levels_size == internal::g_log_levels.size()) {
             throw std::logic_error("Added too many log levels.");
         }

         for (size_t i = 0; i < internal::g_log_levels_size; ++i) {
             if (internal::g_log_levels[i].first == lvl.value) {
                 internal::g_log_levels[i] = {lvl.value, LoggingLevel{lvl, enabled}};
                 return;
             }
         }
         internal::g_log_levels[internal::g_log_levels_size++] = {lvl.value, LoggingLevel{lvl, enabled}};
      }


      void addLogLevel(LEVELS level) {
         addLogLevel(level, true);
      }

      void reset() {
          for (size_t i = 0; i < g3::internal::g_log_level_defaults.size(); ++i) {
              g3::internal::g_log_levels[i] = g3::internal::g_log_level_defaults[i];
          }
          g3::internal::g_log_levels_size = g3::internal::g_log_level_defaults.size();
      }
   } // only_change_at_initialization


   namespace log_levels {

      void setHighest(LEVELS enabledFrom) {
         auto it = std::find_if(internal::g_log_levels.begin(), internal::g_log_levels.end(), [enabledFrom](const auto& a) {
                                    return a.first == enabledFrom.value;
                                });
         if (it == internal::g_log_levels.end()) {
             return;
         }
         for (size_t i = 0; i < g3::internal::g_log_level_defaults.size(); ++i) {
            auto& v = internal::g_log_levels[i];
            if (v.first < enabledFrom.value) {
               disable(v.second.level);
            } else {
               enable(v.second.level);
            }
         }
      }


      void set(LEVELS level, bool enabled) {
         auto it = std::find_if(internal::g_log_levels.begin(), internal::g_log_levels.end(), [level](const auto& a) {
                                    return a.first == level.value;
                                });
         if (it != internal::g_log_levels.end()) {
            *it = {level.value, LoggingLevel{level, enabled}};
         }
      }


      void disable(LEVELS level) {
         set(level, false);
      }

      void enable(LEVELS level) {
         set(level, true);
      }


      void disableAll() {
         for (size_t i = 0; i < g3::internal::g_log_level_defaults.size(); ++i) {
             internal::g_log_levels[i].second.status = false;
         }
      }

      void enableAll() {
         for (size_t i = 0; i < g3::internal::g_log_level_defaults.size(); ++i) {
             internal::g_log_levels[i].second.status = true;
         }
      }


      std::string to_string(std::map<int, g3::LoggingLevel> levelsToPrint) {
         std::string levels;
         for (auto& v : levelsToPrint) {
            levels += "name: " + v.second.level.textString() + " level: " + std::to_string(v.first) + " status: " + std::to_string(v.second.status.value()) + "\n";
         }
         return levels;
      }

      std::string to_string() {
         return to_string(getAll());
      }


      std::map<int, g3::LoggingLevel> getAll() {
         std::map<int, g3::LoggingLevel> log_levels;
         for (size_t i = 0; i < internal::g_log_levels_size; ++i) {
             log_levels.insert(internal::g_log_levels[i]);
         }
         return log_levels;
      }

      // status : {Absent, Enabled, Disabled};
      status getStatus(LEVELS level) {
         for (size_t i = 0; i < internal::g_log_levels_size; ++i) {
             auto& kv = internal::g_log_levels[i];
             if (kv.first == level.value) {
                 return (kv.second.status.get().load() ? status::Enabled : status::Disabled);
             }
         }
         return status::Absent;
      }
   } // log_levels

#endif


   bool logLevel(LEVELS log_level) {
#ifdef G3_DYNAMIC_LOGGING
     auto it = std::find_if(internal::g_log_levels.begin(), internal::g_log_levels.end(), [log_level](const auto& a) {
                                return a.first == log_level.value;
                            });
      if (it == internal::g_log_levels.end()) {
          return false;
      }
      return it->second.status.value();
#endif
      return true;
   }
} // g3
