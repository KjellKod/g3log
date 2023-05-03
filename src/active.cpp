/** ==========================================================================
 * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "g3log/active.hpp"

#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace kjellkod {
   namespace internal {
      native_thread_name getThreadName(std::thread::native_handle_type handle) {
#ifdef WIN32
         static auto getThreadDescription =
            reinterpret_cast<HRESULT(WINAPI *)(HANDLE, PWSTR*)>(::GetProcAddress(
               ::GetModuleHandleW(L"Kernel32.dll"), "GetThreadDescription"));
         if (getThreadDescription == nullptr) {
            return native_thread_name();
         }

         wchar_t *wide_thread_name;
         HRESULT hr = getThreadDescription(handle, &wide_thread_name);
         if (SUCCEEDED(hr)) {
            native_thread_name threadName = wide_thread_name;
            ::LocalFree(wide_thread_name);
            return threadName;
         }

         return native_thread_name();
#else
         // There is no way to get actual thread name size. At least 64 chars
         // buffer required.
         constexpr size_t kEstimateThreadNameSize{64};

         native_thread_name threadName;
         threadName.resize(kEstimateThreadNameSize);

         // Linux, NetBSD, Mac OS X.
         if (!::pthread_getname_np(handle, threadName.data(),
                                   threadName.size())) {
            const size_t terminatorIdx = threadName.find('\0');
            if (terminatorIdx != std::string::npos) {
               threadName.resize(terminatorIdx);
            }
         }

         return threadName;
#endif
      }

      void setThreadName(const native_thread_name &threadNamePrefix) {
         static unsigned threadNo = 0;

#ifdef WIN32
         const native_thread_name threadName(threadNamePrefix + std::to_wstring(threadNo++));
#else
         const native_thread_name threadName(threadNamePrefix + std::to_string(threadNo++));
#endif  // !WIN32
   
#ifdef WIN32
         static auto setThreadDescription =
            reinterpret_cast<HRESULT(WINAPI *)(HANDLE, PCWSTR)>(::GetProcAddress(
               ::GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription"));
         if (setThreadDescription == nullptr) {
            return;
         }

         setThreadDescription(::GetCurrentThread(), threadName.c_str());
#elif defined(__APPLE__)
         // The thread name is a meaningful C language string, whose length is
         // restricted to 64 characters, including the terminating null byte
         // ('\0').
         ::pthread_setname_np(threadName.c_str());
#elif defined(__FreeBSD__)
         ::pthread_set_name_np(::pthread_self(), threadName.c_str());
#elif !defined(__Fuchsia__) && !defined(__EMSCRIPTEN__)
         // The thread name is a meaningful C language string, whose length is
         // restricted to 16 characters, including the terminating null byte
         // ('\0').
         ::pthread_setname_np(::pthread_self(), threadName.c_str());
#else
#error "Please define setThreadName for your platform."
#endif  // !WIN32
      }
   }  // namespace internal
}  // namespace kjellkod