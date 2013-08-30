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
#include "active.h"
#include "g2future.h"


namespace g2 {
  namespace internal {
    typedef std::function<void(LogEntry) > AsyncMessageCall;


    /// The asynchronous Sink has an active object, incoming requests for actions
    //  will be processed in the background by the specific object the Sink represents. 
    // 
    // Ref: send(LogEntry) deals with incoming log entries
    // Ref: send(Call call, Args... args) deals with calls 
    //           to the real sink's API
    template<class T>
    struct Sink : public SinkWrapper {
      std::shared_ptr<T> _real_sink;
      std::unique_ptr<kjellkod::Active> _bg;
      AsyncMessageCall _default_log_call;
      
      

      template<typename DefaultLogCall >
              Sink(std::shared_ptr<T> sink, DefaultLogCall call)
      : SinkWrapper { },
      _real_sink{sink},
      _bg(kjellkod::Active::createActive()),
      _default_log_call(std::bind(call, _real_sink.get(), std::placeholders::_1))
      { }

      virtual ~Sink() {  
        std::cout << "Sink<T> in destructor\n";
        _bg.reset();   
        std::cout << "Sink<T> Active object was reset. in destructor\n";

      }

      void send(LogEntry msg) override {
        _bg->send([this, msg]{_default_log_call(msg);});
      }

      template<typename Call, typename... Args>
              auto send(Call call, Args... args)-> std::future < decltype(bind(call, _real_sink.get(), args...)()) > {
                return g2::spawn_task(std::bind(call, _real_sink.get(), args...), _bg.get());
              }
    };
  }
}


#endif	/* G2SINK_IPP */

