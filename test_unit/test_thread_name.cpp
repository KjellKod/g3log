/** ==========================================================================re
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include <gtest/gtest.h>
#include "g3log/active.hpp"

#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace {
   auto getCurrentThreadHandle() {
#ifdef WIN32
      return ::GetCurrentThread();
#else
      return ::pthread_self();
#endif
   }

   template<typename THandle>
   class ScopedThreadName {
   public:
      ScopedThreadName(THandle handle)
         : old_thread_name_(kjellkod::internal::getThreadName(handle)) {
      }
      ~ScopedThreadName() {
         kjellkod::internal::setThreadName(old_thread_name_);
      }

      ScopedThreadName(ScopedThreadName&) = delete;
      ScopedThreadName(ScopedThreadName&&) = delete;

      ScopedThreadName& operator=(ScopedThreadName&) = delete;
      ScopedThreadName& operator=(ScopedThreadName&&) = delete;

   private:
      const kjellkod::native_thread_name old_thread_name_;
   };
}  // namespace

#ifdef WIN32
#define G3_NATIVE_THREAD_NAME(x) L##x
#else
#define G3_NATIVE_THREAD_NAME(x) x
#endif

TEST(ThreadNaming, GetSetThreadName) {
   const auto currentThread = getCurrentThreadHandle();
   const ScopedThreadName<std::decay_t<decltype(currentThread)>>
      restore_original_thread_name(currentThread);

   const kjellkod::native_thread_name newThreadName =
      G3_NATIVE_THREAD_NAME("g3log test");

   using namespace kjellkod::internal;

   setThreadName(newThreadName);
   // Append thread index.
   EXPECT_EQ(newThreadName + G3_NATIVE_THREAD_NAME("0"),
             getThreadName(currentThread));

   setThreadName(newThreadName);
   // Increment thread index on every attempt.
   EXPECT_EQ(newThreadName + G3_NATIVE_THREAD_NAME("1"),
             getThreadName(currentThread));
}
