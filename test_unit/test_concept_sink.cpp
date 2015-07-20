/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

#include "testing_helpers.h"
#include "g3log/std2_make_unique.hpp"
#include "g3log/sink.hpp"
#include "g3log/sinkwrapper.hpp"
#include "g3log/sinkhandle.hpp"
#include "g3log/logmessage.hpp"


using namespace std;
using namespace std2;
using namespace testing_helpers;

class CoutSink {
   stringstream buffer;
   unique_ptr<ScopedOut> scope_ptr;

   CoutSink() : scope_ptr(std2::make_unique<ScopedOut>(std::cout, &buffer)) {}
public:
   void clear() { buffer.str(""); }
   std::string string() { return buffer.str(); }
   void save(g3::LogMessageMover msg) { std::cout << msg.get().message(); }
   virtual ~CoutSink() final {}
   static std::unique_ptr<CoutSink> createSink() { return std::unique_ptr<CoutSink>(new CoutSink);}
};

struct StringSink {
   std::string raw;
   void append(g3::LogMessageMover entry) { raw.append(entry.get().message());}
   std::string string() {
      return raw;
   }
};


namespace {
   typedef std::shared_ptr<g3::internal::SinkWrapper> SinkWrapperPtr;
}

namespace g3 {

   class Worker {
      std::vector<SinkWrapperPtr> _container; // should be hidden in a pimple with a bg active object
      std::unique_ptr<kjellkod::Active> _bg;

      void bgSave(std::string msg) {
         for (auto& sink : _container) {
            g3::LogMessage message("test", 0, "test", DEBUG);
            message.write().append(msg);
            sink->send(LogMessageMover(std::move(message)));
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
            _container.clear(); });
      }

      void save(std::string msg) {
         _bg->send([this, msg] { bgSave(msg); });
      }
      
      
      template<typename T, typename DefaultLogCall>
      std::unique_ptr< SinkHandle<T> > addSink(std::unique_ptr<T> unique, DefaultLogCall call) {
         auto sink = std::make_shared < internal::Sink<T> > (std::move(unique), call);
         auto add_sink_call = [this, sink] { _container.push_back(sink); };
         auto wait_result = g3::spawn_task(add_sink_call, _bg.get());
         wait_result.wait();

         auto handle = std2::make_unique< SinkHandle<T> >(sink);
         return handle;
      }
   };

} // g3




using namespace g3;
using namespace g3::internal;

TEST(ConceptSink, CreateHandle) {
   Worker worker;
   auto handle = worker.addSink(CoutSink::createSink(), &CoutSink::save);
   ASSERT_NE(nullptr, handle.get());
}

TEST(ConceptSink, OneSink__VerifyMsgIn) {
   Worker worker;
   auto handle = worker.addSink(CoutSink::createSink(), &CoutSink::save);
   worker.save("Hello World!");
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   auto output = handle->call(&CoutSink::string);
   auto content = output.get();
   auto pos = content.find("Hello World!");
   ASSERT_NE(pos, std::string::npos);
}


TEST(ConceptSink, DualSink__VerifyMsgIn) {
   Worker worker;
   auto h1 = worker.addSink(CoutSink::createSink(), &CoutSink::save);
   auto h2 = worker.addSink(std2::make_unique<StringSink>(), &StringSink::append);
   worker.save("Hello World!");


   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   auto first = h1->call(&CoutSink::string);
   auto second = h2->call(&StringSink::string);


   ASSERT_EQ("Hello World!", first.get());
   ASSERT_EQ("Hello World!", second.get());
}

TEST(ConceptSink, DeletedSink__Exptect_badweak_ptr___exception) {
   auto worker = std2::make_unique<Worker>();
   auto h1 = worker->addSink(CoutSink::createSink(), &CoutSink::save);
   worker->save("Hello World!");
   worker.reset();

   auto first = h1->call(&CoutSink::string);
   EXPECT_THROW(first.get(), std::bad_weak_ptr);
}

namespace {
   typedef std::shared_ptr<std::atomic<bool >> AtomicBoolPtr;
   typedef std::shared_ptr<std::atomic<int >> AtomicIntPtr;
   typedef vector<AtomicBoolPtr> BoolList;
   typedef vector<AtomicIntPtr> IntVector;
}

TEST(ConceptSink, OneHundredSinks) {
   BoolList flags;
   IntVector counts;

   size_t NumberOfItems = 100;
   for (size_t index = 0; index < NumberOfItems; ++index) {
      flags.push_back(make_shared < atomic<bool >> (false));
      counts.push_back(make_shared < atomic<int >> (0));
   }

   {
      auto worker = std::unique_ptr<Worker>(new Worker);
      size_t index = 0;
      for (auto& flag : flags) {
         auto& count = counts[index++];
         // ignore the handle
         worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
      }
      worker->save("Hello to 100 receivers :)");
      worker->save("Hello to 100 receivers :)");
   }
   // at the curly brace above the ScopedLogger will go out of scope and all the 
   // 100 logging receivers will get their message to exit after all messages are
   // are processed
   size_t index = 0;
   for (auto& flag : flags) {
      auto& count = counts[index++];
      ASSERT_TRUE(flag->load()) << ", count : " << (index - 1);
      ASSERT_TRUE(2 == count->load()) << ", count : " << (index - 1);
   }

   cout << "test one hundred sinks is finished finished\n";
}


/*
TEST(Sink, OneSink) {
  AtomicBoolPtr flag = make_shared<atomic<bool>>(false);
  AtomicIntPtr count = make_shared<atomic<int>>(0);
    {
    auto worker = std::make_shared<g3LogWorker>();
    worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    worker->save("this message should trigger an atomic increment at the sink");

    EXPECT_FALSE(flag->load());
    EXPECT_TRUE(0 == count->load());
  }
  EXPECT_TRUE(flag->load());
  EXPECT_TRUE(1 == count->load());
}

TEST(Sink, OneSinkWithHandleOutOfScope) {
  AtomicBoolPtr flag = make_shared<atomic<bool>>(false);
  AtomicIntPtr count = make_shared<atomic<int>>(0);
  {
    auto worker = std::make_shared<g3LogWorker>();
    {
       auto handle =   worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    }
    EXPECT_FALSE(flag->load());
    EXPECT_TRUE(0 == count->load());
    worker->save("this message should trigger an atomic increment at the sink");
  }
  EXPECT_TRUE(flag->load());
  EXPECT_TRUE(1 == count->load());
}

//Perfect det här testet triggar felet

typedef vector<AtomicBoolPtr> BoolPtrVector;
typedef vector<AtomicIntPtr> IntPtrVector;
TEST(Sink, OneHundredSinks) {
  BoolPtrVector flags;
  IntPtrVector counts;

  size_t NumberOfItems = 100;
  for (size_t index = 0; index < NumberOfItems; ++index) {
    flags.push_back(make_shared < atomic<bool >> (false));
    counts.push_back(make_shared < atomic<int >> (0));
  }

  {
    auto worker = std::make_shared<g3LogWorker>();
    size_t index = 0;
    for (auto& flag : flags) {
      auto& count = counts[index++];
      // ignore the handle
      worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    }
    worker->save("Hello to 100 receivers :)");
  }

  // at the curly brace above the ScopedLogger will go out of scope and all the 
  // 100 logging receivers will get their message to exit after all messages are
  // are processed // at the curly brace above the ScopedLogger will go out of scope and all the 
  // 100 logging receivers will get their message to exit after all messages are
  // are processed
  size_t index = 0;
  for (auto& flag : flags) {
    auto& count = counts[index++];
    EXPECT_TRUE(flag->load());
    EXPECT_EQ(100, count->load());
    cout << "test one hundred sinks is finished finished\n";
  }
}

 */

