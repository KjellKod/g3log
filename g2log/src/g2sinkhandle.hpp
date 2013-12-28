/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#ifndef G2SINKHANDLE_H
#define	G2SINKHANDLE_H

#include <memory>
#include <functional>
#include "g2sink.hpp"

namespace g2 {
   
  // The Sinkhandle is the client's access point to the specific sink instance.
  // Only through the Sinkhandle can, and should, the real sink's specific API
  // be called. 
  //
  // The real sink will be owned by the g2logger. If the real sink is deleted
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
    template<typename Call, typename... Args>
    auto call(Call call, Args... args) -> decltype(_sink.lock()->send(call, args...)) {
      try {
        std::shared_ptr<internal::Sink<T>> sink(_sink); 
        return sink->send(call, args...);
      } catch (const std::bad_weak_ptr& e) {
        T* t = nullptr;
        typedef decltype(std::bind(call, t, args...)()) PromiseType;
        std::promise<PromiseType> promise;
        promise.set_exception(std::make_exception_ptr(e));
        return std::move(promise.get_future()); 
      }
    }
  };
}
#endif	/* G2SINKHANDLE_H */

