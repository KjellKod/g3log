/* 
 * File:   g2sink.ipp
 * Author: kjell
 *
 * Created on July 21, 2013, 11:27 AM
 */

#ifndef G2SINK_IPP
#define	G2SINK_IPP

#include <memory>
#include <functional>

#include "g2sinkwrapper.h"
#include "active.hpp"
#include "g2future.h"
#include "g2logmessage.hpp"

namespace g2 {
namespace internal {
typedef std::function<void(LogMessageMover) > AsyncMessageCall;

/// The asynchronous Sink has an active object, incoming requests for actions
//  will be processed in the background by the specific object the Sink represents. 
// 
// The Sink will wrap either 
//     a Sink with Message object receiving call
// or  a Sink with a LogEntry (string) receving call
//
// The Sink can also be used through the SinkHandler to call Sink specific function calls
// Ref: send(Message) deals with incoming log entries (converted if necessary to string)
// Ref: send(Call call, Args... args) deals with calls 
//           to the real sink's API

template<class T>
struct Sink : public SinkWrapper {
   std::shared_ptr<T> _real_sink;
   std::unique_ptr<kjellkod::Active> _bg;
   AsyncMessageCall _default_log_call;

   template<typename DefaultLogCall >
   Sink(std::shared_ptr<T> sink, DefaultLogCall call)
   : SinkWrapper (),
   _real_sink{sink},
   _bg(kjellkod::Active::createActive()),
   _default_log_call(std::bind(call, _real_sink.get(), std::placeholders::_1)) {
   }

   Sink(std::shared_ptr<T> sink, void(T::*Call)(std::string) )
   : SinkWrapper(),
   _real_sink {sink},
   _bg(kjellkod::Active::createActive()) {
      auto adapter = std::bind(Call, _real_sink.get(), std::placeholders::_1);
      _default_log_call = [ = ](LogMessageMover m){adapter(m.get().toString());};
   }

   virtual ~Sink() {
      _bg.reset(); // TODO: to remove
   }

   void send(LogMessageMover msg) override {
      _bg->send([this, msg] {
         _default_log_call(msg);
      });
   }

   template<typename Call, typename... Args>
   auto send(Call call, Args... args)-> std::future < decltype(bind(call, _real_sink.get(), args...)()) > {
      return g2::spawn_task(std::bind(call, _real_sink.get(), args...), _bg.get());
   }
};
} // internal
} // g2


#endif	/* G2SINK_IPP */

