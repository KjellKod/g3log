/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <future>
#include <g3log/generated_definitions.hpp>
#include "testing_helpers.h"
#include "g3log/logmessage.hpp"
#include "g3log/logworker.hpp"
#include "g3log/std2_make_unique.hpp"

using namespace testing_helpers;
using namespace std;
TEST(Sink, OneSink) {
  using namespace g3;
  AtomicBoolPtr flag = make_shared < atomic<bool >> (false);
  AtomicIntPtr count = make_shared < atomic<int >> (0);
  {
    auto worker = g3::LogWorker::createLogWorker();
    auto handle = worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    EXPECT_FALSE(flag->load());
    EXPECT_TRUE(0 == count->load());
    LogMessagePtr message{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    message.get()->write().append("this message should trigger an atomic increment at the sink");
    worker->save(message);
  }
  EXPECT_TRUE(flag->load());
  EXPECT_TRUE(1 == count->load());
}

TEST(Sink, OneSinkRemove) {
  using namespace g3;
  AtomicBoolPtr flag = make_shared < atomic<bool >> (false);
  AtomicIntPtr count = make_shared < atomic<int >> (0);
  {
    auto worker = g3::LogWorker::createLogWorker();
    auto handle = worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    EXPECT_FALSE(flag->load());
    EXPECT_TRUE(0 == count->load());

    LogMessagePtr message1{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    message1.get()->write().append("this message should trigger an atomic increment at the sink");
    worker->save(message1);

    worker->removeSink(std::move(handle));
    EXPECT_TRUE(flag->load());
    EXPECT_TRUE(1 == count->load());

    LogMessagePtr message2{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    message2.get()->write().append("this message is issued after all sinks are removed");
    worker->save(message2);
  }
  EXPECT_TRUE(1 == count->load());
}




namespace {
  using AtomicBoolPtr =  std::shared_ptr<std::atomic<bool>>;
  using AtomicIntPtr =  std::shared_ptr<std::atomic<int>>;
  using BoolList =  vector<AtomicBoolPtr>;
  using IntVector =  vector<AtomicIntPtr>;


  size_t countDestroyedFlags(BoolList& flags) {
    size_t destroyed_count = 0;
    for (auto& flag : flags) {
      if (flag->load()) {
        ++destroyed_count;
      }
    }
    return destroyed_count;
  }

  bool expectedMessagesPerSink(const size_t expected, IntVector& messages) {
    bool result = true;
    for (auto& count : messages) {
      result = result && (count->load() == expected);
    }
    return result;
  }

  size_t countTotalMessages(IntVector& messages) {
    size_t total_count = 0;
    for (auto& count : messages) {
      total_count += count->load();
    }
    return total_count;
  }
} // namespace

TEST(ConceptSink, OneHundredSinks) {
  using namespace g3;
  BoolList flags;
  IntVector counts;

  size_t kNumberOfItems = 100;
  for (size_t index = 0; index < kNumberOfItems; ++index) {
    flags.push_back(make_shared < atomic<bool >> (false));
    counts.push_back(make_shared < atomic<int >> (0));
  }

  {
    RestoreFileLogger logger{"./"};
    g3::LogWorker* worker = logger._scope->get(); //g3LogWorker::createLogWorker();
    size_t index = 0;
    for (auto& flag : flags) {
      auto& count = counts[index++];
      // ignore the handle
      worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg);
    }
    LogMessagePtr message1{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    LogMessagePtr message2{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    auto& write1 = message1.get()->write();
    write1.append("Hello to 100 receivers :)");
    worker->save(message1);

    auto& write2 = message2.get()->write();
    write2.append("Hello to 100 receivers :)");
    worker->save(message2);
    logger.reset();
  }
  // at the curly brace above the ScopedLogger will go out of scope and all the
  // 100 logging receivers will get their message to exit after all messages are
  // are processed
  auto destroyed = countDestroyedFlags(flags);
  auto messages = countTotalMessages(counts);
  ASSERT_TRUE(destroyed == kNumberOfItems);
  EXPECT_TRUE(expectedMessagesPerSink(2, counts));
  EXPECT_EQ(2 * kNumberOfItems, messages);
}


TEST(ConceptSink, OneHundredSinksRemoved) {
  using namespace g3;
  BoolList flags;
  IntVector counts;

  size_t kNumberOfItems = 100;
  for (size_t index = 0; index < kNumberOfItems; ++index) {
    flags.push_back(make_shared < atomic<bool >> (false));
    counts.push_back(make_shared < atomic<int >> (0));
  }

  {
    RestoreFileLogger logger{"./"};
    g3::LogWorker* worker = logger._scope->get(); //g3LogWorker::createLogWorker();
    size_t index = 0;

    using SinkHandleT = std::unique_ptr<g3::SinkHandle<ScopedSetTrue>>;
    std::vector<SinkHandleT> sink_handles;
    sink_handles.reserve(kNumberOfItems);

    for (auto& flag : flags) {
      auto& count = counts[index++];
      // ignore the handle
      sink_handles.push_back(worker->addSink(std2::make_unique<ScopedSetTrue>(flag, count), &ScopedSetTrue::ReceiveMsg));
    }

    LogMessagePtr message{std2::make_unique<LogMessage>("test", 0, "test", DEBUG)};
    auto& write = message.get()->write();
    write.append("Hello to 100 receivers :)");
    worker->save(message);

    for (auto&x : sink_handles) {
      worker->removeSink(std::move(x));
    }
    EXPECT_EQ(kNumberOfItems, countDestroyedFlags(flags));
    EXPECT_EQ(kNumberOfItems, countTotalMessages(counts));
    // at the curly brace above the ScopedLogger will go out of scope. The logging sink removal
    // is synchronous and all the sinks are guaranteed to have received the message before the sink is removed.
  }
}





struct VoidReceiver {
  std::atomic<int>* _atomicCounter;
  explicit VoidReceiver(std::atomic<int>* counter) : _atomicCounter(counter) {}

  void receiveMsg(std::string msg) { /*ignored*/}
  void incrementAtomic() {
    (*_atomicCounter)++;
  }
};

TEST(ConceptSink, VoidCall__NoCall_ExpectingNoAdd) {
  std::atomic<int> counter{0};
  {
    std::unique_ptr<g3::LogWorker> worker{g3::LogWorker::createLogWorker()};
    auto handle = worker->addSink(std2::make_unique<VoidReceiver>(&counter), &VoidReceiver::receiveMsg);
  }
  EXPECT_EQ(counter, 0);
}

TEST(ConceptSink, VoidCall__OneCall_ExpectingOneAdd) {
  std::atomic<int> counter{0};
  {
    std::unique_ptr<g3::LogWorker> worker{g3::LogWorker::createLogWorker()};
    auto handle = worker->addSink(std2::make_unique<VoidReceiver>(&counter), &VoidReceiver::receiveMsg);
    std::future<void> ignored = handle->call(&VoidReceiver::incrementAtomic);
  }
  EXPECT_EQ(counter, 1);
}

TEST(ConceptSink, VoidCall__TwoCalls_ExpectingTwoAdd) {
  std::atomic<int> counter{0};
  {
    std::unique_ptr<g3::LogWorker> worker{g3::LogWorker::createLogWorker()};
    auto handle = worker->addSink(std2::make_unique<VoidReceiver>(&counter), &VoidReceiver::receiveMsg);
    auto  voidFuture1 = handle->call(&VoidReceiver::incrementAtomic);
    auto  voidFuture2 = handle->call(&VoidReceiver::incrementAtomic);
    voidFuture1.wait();
    EXPECT_TRUE(counter >= 1);
  }
  EXPECT_EQ(counter, 2);
}


struct IntReceiver {
  std::atomic<int>* _atomicCounter;
  explicit IntReceiver(std::atomic<int>* counter) : _atomicCounter(counter) {}

  void receiveMsgDoNothing(std::string msg) { /*ignored*/}
  void receiveMsgIncrementAtomic(std::string msg) { incrementAtomic(); }
  int incrementAtomic() {
    (*_atomicCounter)++;
    int value = *_atomicCounter;
    return value;
  }
};

TEST(ConceptSink, IntCall__TwoCalls_ExpectingTwoAdd) {
  std::atomic<int> counter{0};
  {
    std::unique_ptr<g3::LogWorker> worker{g3::LogWorker::createLogWorker()};
    auto handle = worker->addSink(std2::make_unique<IntReceiver>(&counter), &IntReceiver::receiveMsgDoNothing);
    std::future<int> intFuture1 = handle->call(&IntReceiver::incrementAtomic);
    EXPECT_EQ(intFuture1.get(), 1);
    EXPECT_EQ(counter, 1);

    auto intFuture2 = handle->call(&IntReceiver::incrementAtomic);
    EXPECT_EQ(intFuture2.get(), 2);

  }
  EXPECT_EQ(counter, 2);
}



void DoLogCalls(std::atomic<bool>*  doWhileTrue, size_t counter) {
  while (doWhileTrue->load()) {
    LOG(INFO) << "Calling from #" << counter;
    std::this_thread::yield();
  }
}


TEST(ConceptSink, CannotCallSpawnTaskOnNullptrWorker) {
  auto FailedHelloWorld = [] { std::cout << "Hello World" << std::endl; };
  kjellkod::Active* active = nullptr;
  auto failed = g3::spawn_task(FailedHelloWorld, active);
  EXPECT_ANY_THROW(failed.get());
}

TEST(ConceptSink, DISABLED_AggressiveThreadCallsDuringShutdown) {
  std::atomic<bool> keepRunning{true};

  std::vector<std::thread> threads;
  const size_t numberOfThreads = std::thread::hardware_concurrency() * 4;
  threads.reserve(numberOfThreads);

  g3::internal::shutDownLogging();

  // Avoid annoying printouts at log shutdown
  stringstream cerr_buffer;
  testing_helpers::ScopedOut guard1(std::cerr, &cerr_buffer);

  // these threads will continue to write to a logger
  // while the receiving logger is instantiated, and destroyed repeatedly
  for (size_t caller = 0; caller < numberOfThreads; ++ caller) {
    threads.push_back(std::thread(DoLogCalls, &keepRunning, caller));
  }


  std::atomic<int> atomicCounter{0};
  size_t numberOfCycles = 25;
  std::cout << "Create logger, delete active logger, " << numberOfCycles << " times\n\tWhile " << numberOfThreads << " threads are continously doing LOG calls" << std::endl;
  std::cout << "Create/Destroy Times #";
  for (size_t create = 0; create < numberOfCycles; ++create) {
    std::cout << create << " ";

    std::unique_ptr<g3::LogWorker> worker{g3::LogWorker::createLogWorker()};
    auto handle = worker->addSink(std2::make_unique<IntReceiver>(&atomicCounter), &IntReceiver::receiveMsgIncrementAtomic);
    g3::initializeLogging(worker.get());

    // wait till some LOGS streaming in
    atomicCounter = 0;
    while (atomicCounter.load() < 10) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  } // g3log worker exists:  1) shutdownlogging 2) flush of queues and shutdown of sinks


  // exit the threads
  keepRunning = false;
  for (auto& t : threads) {
    t.join();
  }
  std::cout << "\nAll threads are joined " << std::endl;
}


