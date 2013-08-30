#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
 
#include "testing_helpers.h"
#include "g2logworker.h"

namespace {
  g2LogWorker* g_logger_ptr = nullptr;
}


using namespace testing_helpers;
using namespace std;

TEST(Sink, TestSetup) {
  ScopedLogger scope;
  ASSERT_EQ(g_logger_ptr, scope._previousWorker);
}

TEST(Sink, OneSink) {
  AtomicBoolPtr flag = make_shared<atomic<bool>>(false);
  AtomicIntPtr count = make_shared<atomic<int>>(0);
  {
    auto worker = g2LogWorker::createWithNoSink();
    auto handle = worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    EXPECT_FALSE(flag->load());
    EXPECT_TRUE(0 == count->load());
    worker->save("this message should trigger an atomic increment at the sink");
  }
  EXPECT_TRUE(flag->load());
  EXPECT_TRUE(1 == count->load());
}


//Perfect det här testet triggar felet
typedef vector<AtomicBoolPtr> BoolPtrVectorX;
typedef vector<AtomicIntPtr> IntPtrVectorX;
TEST(Sink, OneHundredSinks) {
  BoolPtrVectorX flags;
  IntPtrVectorX counts;
  
  size_t NumberOfItems = 1;
  for(size_t index = 0; index < NumberOfItems; ++index) {
    flags.push_back(make_shared<atomic<bool>>(false));
    counts.push_back(make_shared<atomic<int>>(0));
  }
  
  
  {
    auto worker = g2LogWorker::createWithNoSink();  
    for(size_t index = 0; index < NumberOfItems; ++index) {
      AtomicBoolPtr flag = flags[index];
      AtomicIntPtr count = counts[index];
      // ignore the handle
      worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    }
    worker->save("Hello to 100 receivers");
  }
  // at the curly brace above the ScopedLogger will go out of scope and all the 
  // 100 logging receivers will get their message to exit after all messages are
  // are processed
  for(size_t index = NumberOfItems-1; index >=0; --index) {
    AtomicBoolPtr flag = flags[index];
    AtomicIntPtr count = counts[index];
    EXPECT_TRUE(flag->load());
    EXPECT_TRUE(1 == count->load());
  }
}     

//    
//    for(size_t = 0; )
//    auto handle = scope.get()->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
//    EXPECT_FALSE(flag);
//    EXPECT_TRUE(0 == count);
//    LOG(INFO) << "this message should trigger an atomic increment at the sink";
  
  //EXPECT_TRUE(flag);
  //EXPECT_TRUE(1 == count);


int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  auto logger = g2LogWorker::createWithNoSink();
  g_logger_ptr = logger.get();
  g2::initializeLogging(logger.get());
  int return_value = RUN_ALL_TESTS();
  std::cout << "FINISHED WITH THE TESTING" << std::endl;
  return return_value;
}