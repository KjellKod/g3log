#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "testing_helpers.h"
#include "std2_make_unique.hpp"
#include "g2sink.h"
#include "g2sinkwrapper.h"
#include "g2sinkhandle.h"
#include "g2logmessage.hpp"


using namespace std;
using namespace std2;

class CoutSink {
  stringstream buffer;
  unique_ptr<ScopedCout> scope_ptr;
  
  CoutSink() : scope_ptr(std2::make_unique<ScopedCout>(&buffer)) {  }
public:
  void clear() {  buffer.str("");  }
  std::string string() {    return buffer.str();  }
  void save(g2::internal::LogEntry msg) { std::cout << msg;  }

  virtual ~CoutSink() final { }

  static std::unique_ptr<CoutSink> createSink() 
  { return std::unique_ptr<CoutSink>(new CoutSink);  }
};


namespace {
  typedef std::shared_ptr<g2::internal::SinkWrapper> SinkWrapperPtr;
  typedef g2::internal::LogEntry LogEntry;
}

namespace g2 {

  class Worker {
    std::vector<SinkWrapperPtr> _container; // should be hidden in a pimple with a bg active object
    std::unique_ptr<kjellkod::Active> _bg;

    void bgSave(LogEntry msg) {
      for (auto& sink : _container) {
        sink->send(msg);
      }
    }

  public:

    Worker() : _bg {
      kjellkod::Active::createActive()
    }
    {
    }

    ~Worker() {
      _bg->send([this] {
        _container.clear();
      });
    }

    void save(LogEntry msg) {
      _bg->send([this, msg] {
        bgSave(msg);
      });
    } // will this be copied?
    //this is guaranteed to work std::bind(&Worker::bgSave, this, msg));   }

    template<typename T, typename DefaultLogCall>
    std::unique_ptr< SinkHandle<T> > addSink(std::unique_ptr<T> unique, DefaultLogCall call) {
      auto shared = std::shared_ptr<T>(unique.release());
      auto sink = std::make_shared < internal::Sink<T> > (shared, call);
      auto add_sink_call = [this, sink] { _container.push_back(sink);
      
      };
      auto wait_result = g2::spawn_task(add_sink_call, _bg.get());
      wait_result.wait();

      auto handle = std2::make_unique< SinkHandle<T> >(sink);
      return handle;
    }
  };
  
} // g2
  
 
    
  using namespace g2;
  using namespace g2::internal;
    
  TEST(Sink, CreateHandle) {
    Worker worker;    
    auto handle = worker.addSink(CoutSink::createSink(), &CoutSink::save); 
    ASSERT_NE(nullptr, handle.get());
  }
  
  TEST(Sink, OneSink__VerifyMsgIn) {
    Worker worker;
    auto handle = worker.addSink(CoutSink::createSink(), &CoutSink::save);
    worker.save("Hello World!");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto output = handle->call(&CoutSink::string);
    ASSERT_EQ("Hello World!", output.get());
  }
  
  struct StringSink {
    std::string raw;
    void append(LogEntry entry) { raw.append(entry); }
    std::string string(){return raw; }
  };
  
  
  TEST(Sink, DualSink__VerifyMsgIn) {
    Worker worker;
    auto h1 = worker.addSink(CoutSink::createSink(), &CoutSink::save);
    auto h2 = worker.addSink(std2::make_unique<StringSink>(), &StringSink::append);
    worker.save("Hello World!");
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto first =  h1->call(&CoutSink::string);
    auto second = h2->call(&StringSink::string);
    
    
    ASSERT_EQ("Hello World!", first.get());
    ASSERT_EQ("Hello World!", second.get());
  }
  
  
  
  TEST(Sink, DeletedSink__Exptect_badweak_ptr___exception) {
  auto worker = std2::make_unique<Worker>();
  auto h1 = worker->addSink(CoutSink::createSink(), &CoutSink::save);
  worker->save("Hello World!");
  worker.reset();

  auto first = h1->call(&CoutSink::string);
  EXPECT_THROW(first.get(), std::bad_weak_ptr);
}
