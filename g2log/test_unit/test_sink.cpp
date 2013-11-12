#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

#include "testing_helpers.h"
#include "g2logmessage.hpp"
#include "g2logworker.hpp"

using namespace testing_helpers;
using namespace std;
TEST(Sink, OneSink) {
using namespace g2;
   AtomicBoolPtr flag = make_shared < atomic<bool >> (false);
   AtomicIntPtr count = make_shared < atomic<int >> (0);
   {
      auto worker = g2::LogWorker::createWithNoSink();
      auto handle = worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
      EXPECT_FALSE(flag->load());
      EXPECT_TRUE(0 == count->load());
      //worker->save("this message should trigger an atomic increment at the sink");
      LogMessagePtr message{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
      message.get()->stream() << "this message should trigger an atomic increment at the sink";
      worker->save(message);
   }
   EXPECT_TRUE(flag->load());
   EXPECT_TRUE(1 == count->load());
}


namespace {
   typedef std::shared_ptr<std::atomic<bool >> AtomicBoolPtr;
   typedef std::shared_ptr<std::atomic<int >> AtomicIntPtr;
   typedef vector<AtomicBoolPtr> BoolList;
   typedef vector<AtomicIntPtr> IntVector;
}

TEST(ConceptSink, OneHundredSinks) {
   using namespace g2;
   BoolList flags;
   IntVector counts;

   size_t NumberOfItems = 100;
   for (size_t index = 0; index < NumberOfItems; ++index) {
      flags.push_back(make_shared < atomic<bool >> (false));
      counts.push_back(make_shared < atomic<int >> (0));
   }

   {
      RestoreFileLogger logger{"/tmp"};
      g2::LogWorker* worker = logger._scope->get(); //g2LogWorker::createWithNoSink();
      size_t index = 0;
      for (auto& flag : flags) {
         auto& count = counts[index++];
         // ignore the handle
         worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
      }
      LOG(DEBUG) << "start message";
      LogMessagePtr message1{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
      LogMessagePtr message2{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
      auto& stream1 = message1.get()->stream();
      stream1 << "Hello to 100 receivers :)";
      worker->save(message1);
      
      auto& stream2 = message2.get()->stream();
      stream2 << "Hello to 100 receivers :)";
      worker->save(message2);
      //worker->save("Hello to 100 receivers :)");
      //worker->save("Hello to 100 receivers :)");
      LOG(INFO) << "end message";
   }
   // at the curly brace above the ScopedLogger will go out of scope and all the 
   // 100 logging receivers will get their message to exit after all messages are
   // are processed
   size_t index = 0;
   for (auto& flag : flags) {
      auto& count = counts[index++];
      ASSERT_TRUE(flag->load()) << ", count : " << (index - 1);
      ASSERT_TRUE(4 == count->load()) << ", count : " << (index - 1);
   }

   cout << "test one hundred sinks is finished finished\n";
}
