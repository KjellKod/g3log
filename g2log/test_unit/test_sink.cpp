#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
 
#include "testing_helpers.h"


namespace {
  g2LogWorker* g_logger_ptr = nullptr;
  
  struct ScopedSetTrue{
    std::atomic<bool>& _flag;
    std::atomic<int>& _count;
    
    explicit ScopedSetTrue(std::atomic<bool>& flag, std::atomic<int>& count) 
    : _flag(flag), _count(count) {}
    
    void ReceiveMsg(g2::internal::LogEntry message){ 
     std::chrono::milliseconds wait{100};
     std::this_thread::sleep_for(wait);
      ++_count;
    }
    ~ScopedSetTrue(){_flag=true;}
  };
}


using namespace testing_helpers;
using namespace std;

TEST(Sink, TestSetup) {
  ScopedLogger scope;
  ASSERT_EQ(g_logger_ptr, scope._previousWorker);
}

TEST(Sink, OneSink) {
  std::atomic<bool> flag{false};
  std::atomic<int> count{0};
  {
    ScopedLogger scope;
    auto handle = scope.get()->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    EXPECT_FALSE(flag);
    EXPECT_TRUE(0 == count);
    LOG(INFO) << "this message should trigger an atomic increment at the sink";
  }
  EXPECT_TRUE(flag);
  EXPECT_TRUE(1 == count);
}


Perfect det här testet triggar felet
typedef vector<shared_ptr<atomic<bool>>> BoolPtrVector;
typedef vector<shared_ptr<atomic<int>>> IntPtrVector;
TEST(Sink, OneHundredSinks) {
  BoolPtrVector flags;
  IntPtrVector counts;
  
  size_t NumberOfItems = 100;
  for(size_t index = 0; index < NumberOfItems; ++index) {
    flags.push_back(make_shared<atomic<bool>>(false));
    counts.push_back(make_shared<atomic<int>>(0));
  }
  
  {
    ScopedLogger scope;
    size_t NumberOfItems = 100;
    for(size_t index = 0; index < NumberOfItems; ++index) {
      atomic<bool>& flag = *(flags[index].get());
      atomic<int>& count = *(counts[index].get());
      // ignore the handle
      scope.get()->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    }
    LOG(INFO) << "Hello to 100 receivers :)";
  }
  // at the curly brace above the ScopedLogger will go out of scope and all the 
  // 100 logging receivers will get their message to exit after all messages are
  // are processed
  for(size_t index = NumberOfItems-1; index >=0; --index) {
    atomic<bool>& flag = *(flags[index].get());
    atomic<int>& count = *(counts[index].get());
    EXPECT_TRUE(flag);
    EXPECT_TRUE(1 == count);
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