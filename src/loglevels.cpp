/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/loglevels.hpp"
#include <atomic>
#include <cassert>
#include <map>

namespace {
   namespace {
      /// As suggested in: http://stackoverflow.com/questions/13193484/how-to-declare-a-vector-of-atomic-in-c
      struct atomicbool {
       private:
         std::atomic<bool> value_;
       public:
         atomicbool(): value_ {false} {}
         atomicbool(const bool &value): value_ {value} {}
         atomicbool(const std::atomic<bool> &value) : value_ {value.load(std::memory_order_acquire)} {}
         atomicbool(const atomicbool &other): value_ {other.value_.load(std::memory_order_acquire)} {}
         atomicbool &operator=(const atomicbool &other) {
            value_.store(other.value_.load(std::memory_order_acquire), std::memory_order_release);
            return *this;
         }
         bool value() {return value_.load(std::memory_order_acquire);}
         std::atomic<bool>& get() {return value_;}
      };

   } // anonymous

}
namespace g3 {
   namespace internal {
      bool wasFatal(const LEVELS &level) {
         return level.value >= FATAL.value;
      }

#ifdef G3_DYNAMIC_LOGGING
      std::map<int, atomicbool> g_log_level_status = {{g3::kDebugVaulue, true}, {INFO.value, true}, {WARNING.value, true}, {FATAL.value, true}};
#endif
   } // internal

#ifdef G3_DYNAMIC_LOGGING
   namespace only_change_at_initialization {
      void setLogLevel(LEVELS log_level, bool enabled) {
         int level = log_level.value;
         internal::g_log_level_status[level].get().store(enabled, std::memory_order_release);
      }

      std::string printLevels() {
         std::string levels;
         for (auto& v : internal::g_log_level_status) {
            levels += "value: " + std::to_string(v.first) + " status: " + std::to_string(v.second.value()) + "\n";
         }
         return levels;
      }

      void reset() {
         internal::g_log_level_status.clear();
         internal::g_log_level_status = std::map<int, atomicbool>{{g3::kDebugVaulue, true}, {INFO.value, true}, {WARNING.value, true}, {FATAL.value, true}};
      }
   } // only_change_at_initialization
#endif

   bool logLevel(LEVELS log_level) {
#ifdef G3_DYNAMIC_LOGGING
      int level = log_level.value;
      bool status = internal::g_log_level_status[level].value();
      return status;
#endif
      return true;
   }
} // g3
