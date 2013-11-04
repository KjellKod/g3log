#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

#include "testing_helpers.h"
#include "g2logmessageimpl.hpp"
#include "g2logmessage.hpp"
#include "g2logworker.hpp"

namespace {
   g2LogWorker* g_logger_ptr = nullptr;
}


using namespace testing_helpers;
using namespace std;

TEST(Sink, TestSetup) {
   {
      ScopedLogger scope;
      ASSERT_EQ(g_logger_ptr, scope._previousWorker);
   }
   ScopedLogger scope;
   ASSERT_EQ(g_logger_ptr, scope._previousWorker);
}

TEST(Sink, OneSink) {
   AtomicBoolPtr flag = make_shared < atomic<bool >> (false);
   AtomicIntPtr count = make_shared < atomic<int >> (0);
   {
      auto worker = g2LogWorker::createWithNoSink();
      auto handle = worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
      EXPECT_FALSE(flag->load());
      EXPECT_TRUE(0 == count->load());
      //worker->save("this message should trigger an atomic increment at the sink");
      g2::LogMessage message(std::make_shared<g2::LogMessageImpl>("test", 0, "test", DEBUG));
      message.stream() << "this message should trigger an atomic increment at the sink";
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
   BoolList flags;
   IntVector counts;

   size_t NumberOfItems = 100;
   for (size_t index = 0; index < NumberOfItems; ++index) {
      flags.push_back(make_shared < atomic<bool >> (false));
      counts.push_back(make_shared < atomic<int >> (0));
   }

   {
      ScopedLogger scope;
      auto worker = scope.get(); //g2LogWorker::createWithNoSink();
      size_t index = 0;
      for (auto& flag : flags) {
         auto& count = counts[index++];
         // ignore the handle
         worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
      }
      LOG(DEBUG) << "start message";
      g2::LogMessage message(std::make_shared<g2::LogMessageImpl>("test", 0, "test", DEBUG));
      auto& stream = message.stream();
      stream << "Hello to 100 receivers :)";
      worker->save(message);
      stream << "Hello to 100 receivers :)";
      worker->save(message);
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

//    
//    for(size_t = 0; )
//    auto handle = scope.get()->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
//    EXPECT_FALSE(flag);
//    EXPECT_TRUE(0 == count);
//    LOG(INFO) << "this message should trigger an atomic increment at the sink";

//EXPECT_TRUE(flag);
//EXPECT_TRUE(1 == count);

int main(int argc, char *argv[]) {
   testing::InitGoogleTest(&argc, argv);
   auto logger = g2LogWorker::createWithNoSink();
   g_logger_ptr = logger.get();
   g2::initializeLogging(logger.get());
   int return_value = RUN_ALL_TESTS();
   std::cout << "FINISHED WITH THE TESTING" << std::endl;
   return return_value;
}