/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#pragma once

#include "g3log/sink.hpp"

#include <memory>
#include <type_traits>

namespace g3 {

   // The Sinkhandle is the client's access point to the specific sink instance.
   // Only through the Sinkhandle can, and should, the real sink's specific API
   // be called.
   //
   // The real sink will be owned by g3log. If the real sink is deleted
   // calls to sink's API through the SinkHandle will return an exception embedded
   // in the resulting future. Ref: SinkHandle::call
   template<class T>
   class SinkHandle {
      std::weak_ptr<internal::Sink<T>> _sink;

    public:
      SinkHandle(std::shared_ptr<internal::Sink<T>> sink)
         : _sink(sink) {}

      ~SinkHandle() {}


      // Asynchronous call to the real sink. If the real sink is already deleted
      // the returned future will contain a bad_weak_ptr exception instead of the
      // call result.
      template<typename AsyncCall, typename... Args>
      auto call(AsyncCall func , Args&& ... args) -> std::future<std::invoke_result_t<decltype(func), T, Args...>> {
         try {
            std::shared_ptr<internal::Sink<T>> sink(_sink);
            return sink->async(func, std::forward<Args>(args)...);
         } catch (const std::bad_weak_ptr& e) {
            typedef std::invoke_result_t<decltype(func), T, Args...> PromiseType;
            std::promise<PromiseType> promise;
            promise.set_exception(std::make_exception_ptr(e));
            return std::move(promise.get_future());
         }
      }

      /// Get weak_ptr access to the sink(). Make sure to check that the returned pointer is valid,
      /// auto p = sink(); auto ptr = p.lock(); if (ptr) { .... }
      /// ref: https://en.cppreference.com/w/cpp/memory/weak_ptr/lock
      std::weak_ptr<internal::Sink<T>> sink() {
         return _sink.lock();
      }
   };

} // g3


