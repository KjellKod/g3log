/** ==========================================================================re
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include <gtest/gtest.h>
#include "g3log/g3log.hpp"
#include "g3log/generated_definitions.hpp"
#include "g3log/loglevels.hpp"
#include "g3log/logworker.hpp"
#include "testing_helpers.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <exception>
#include <iomanip>
#include <memory>
#include <string>
#include <thread>

namespace {
   const std::string log_directory = "./";
   const std::string t_info = "test INFO ";
   const std::string t_info2 = "test INFO 123";
   const std::string t_debug = "test DEBUG ";
   const std::string t_debug3 = "test DEBUG 1.123456";
   const std::string t_warning = "test WARNING ";
   const std::string t_warning3 = "test WARNING yello";

   std::atomic<size_t> g_fatal_counter = {0};
   void fatalCounter() {
      ++g_fatal_counter;
   }

}  // end anonymous namespace

using namespace testing_helpers;
TEST(LogTest, LOGF__FATAL) {
   RestoreFileLogger logger(log_directory);
   ASSERT_FALSE(mockFatalWasCalled());
   LOGF(FATAL, "This message should throw %d", 0);
   EXPECT_TRUE(mockFatalWasCalled());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "This message should throw 0")) << "\n****" << mockFatalMessage();
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "FATAL"));

   auto file_content = logger.resetAndRetrieveContent();
   EXPECT_TRUE(verifyContent(file_content, "This message should throw 0")) << "\n****" << file_content;
   EXPECT_TRUE(verifyContent(file_content, "FATAL"));
}

#ifndef DISABLE_FATAL_SIGNALHANDLING

TEST(LogTest, FatalSIGTERM__UsingDefaultHandler) {
   RestoreFileLogger logger(log_directory);
   g_fatal_counter.store(0);
   g3::setFatalPreLoggingHook(fatalCounter);
   raise(SIGTERM);
   logger.reset();
   EXPECT_EQ(g_fatal_counter.load(), size_t{1});
}

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
namespace {
   std::atomic<size_t> customFatalCounter = {0};
   std::atomic<int> lastEncounteredSignal = {0};
   void customSignalHandler(int signal_number, siginfo_t* info, void* unused_context) {
      lastEncounteredSignal.store(signal_number);
      ++customFatalCounter;
   }
   void installCustomSIGTERM() {
      struct sigaction action;
      memset(&action, 0, sizeof(action));
      sigemptyset(&action.sa_mask);
      action.sa_sigaction = &customSignalHandler;
      action.sa_flags = SA_SIGINFO;
      sigaction(SIGTERM, &action, nullptr);
   }

   std::atomic<bool> oldSigTermCheck = {false};
   void customOldSignalHandler(int signal_number, siginfo_t* info, void* unused_context) {
      lastEncounteredSignal.store(signal_number);
      oldSigTermCheck.store(true);
   }
   void installCustomOldSIGTERM() {
      struct sigaction action;
      memset(&action, 0, sizeof(action));
      sigemptyset(&action.sa_mask);
      action.sa_sigaction = &customOldSignalHandler;
      action.sa_flags = SA_SIGINFO;
      sigaction(SIGTERM, &action, nullptr);
   }

}  // namespace

// Override of signal handling and testing of it should be fairly easy to port to windows
// ref: https://github.com/KjellKod/g3log/blob/master/src/crashhandler_windows.cpp
// what is missing is the override of signals and custom installation of signals
// ref: https://github.com/KjellKod/g3log/blob/master/src/crashhandler_unix.cpp
//      functions: void restoreFatalHandlingToDefault()
//                 void overrideSetupSignals(const std::map<int, std::string> overrideSignals)
//                 void restoreSignalHandler(int signal_number)
//
// It would require some adding of unit test (see the test below)
// and good Windows experience. Since I am not currently working much on the Windows
// side I am reaching out to the community for this one:
//
//
// For the test to work the following code should be added in this test
//void customSignalHandler(int signal_number) {
//	lastEncounteredSignal.store(signal_number);
//	++customFatalCounter;
//}
//
//void installCustomSIGTERM() {
//	ASSERT_TRUE(SIG_ERR != signal(SIGTERM, customSignalHandler));
//}

TEST(LogTest, FatalSIGTERM__UsingCustomHandler) {
   RestoreFileLogger logger(log_directory);
   g_fatal_counter.store(0);
   g3::setFatalPreLoggingHook(fatalCounter);
   installCustomSIGTERM();
   g3::overrideSetupSignals({{SIGABRT, "SIGABRT"}, {SIGFPE, "SIGFPE"}, {SIGILL, "SIGILL"}});

   installCustomSIGTERM();
   EXPECT_EQ(customFatalCounter.load(), size_t{0});
   EXPECT_EQ(lastEncounteredSignal.load(), 0);

   raise(SIGTERM);
   logger.reset();
   EXPECT_EQ(g_fatal_counter.load(), size_t{0});
   EXPECT_EQ(lastEncounteredSignal.load(), SIGTERM);
   EXPECT_EQ(customFatalCounter.load(), size_t{1});
}

TEST(LogTest, FatalSIGTERM__VerifyingOldCustomHandler) {
   RestoreFileLogger logger(log_directory);
   g_fatal_counter.store(0);
   customFatalCounter.store(0);
   lastEncounteredSignal.store(0);

   g3::setFatalPreLoggingHook(fatalCounter);
   installCustomOldSIGTERM();
   g3::overrideSetupSignals({{SIGABRT, "SIGABRT"}, {SIGFPE, "SIGFPE"}, {SIGILL, "SIGILL"}, {SIGTERM, "SIGTERM"}});
   g3::restoreSignalHandler(SIGTERM);  // revert SIGTERM installation

   EXPECT_EQ(customFatalCounter.load(), size_t{0});
   EXPECT_EQ(lastEncounteredSignal.load(), 0);
   EXPECT_FALSE(oldSigTermCheck.load());
   raise(SIGTERM);
   logger.reset();
   EXPECT_EQ(g_fatal_counter.load(), size_t{0});
   EXPECT_EQ(lastEncounteredSignal.load(), SIGTERM);
   EXPECT_TRUE(oldSigTermCheck.load());
}

#endif  // DISABLE_FATAL_SIGNALHANDLING
#endif  // !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))

TEST(LogTest, LOG_preFatalLogging_hook) {
   {
      RestoreFileLogger logger(log_directory);
      ASSERT_FALSE(mockFatalWasCalled());
      g_fatal_counter.store(0);
      g3::setFatalPreLoggingHook(fatalCounter);
      LOG(FATAL) << "This message is fatal";
      logger.reset();
      EXPECT_EQ(g_fatal_counter.load(), size_t{1});
   }
   {
      // Now with no fatal pre-logging-hook
      RestoreFileLogger logger(log_directory);
      ASSERT_FALSE(mockFatalWasCalled());
      g_fatal_counter.store(0);
      LOG(FATAL) << "This message is fatal";
      EXPECT_EQ(g_fatal_counter.load(), size_t{0});
   }
}

TEST(LogTest, LOG_FATAL) {
   RestoreFileLogger logger(log_directory);
   ASSERT_FALSE(mockFatalWasCalled());

   LOG(FATAL) << "This message is fatal";
   EXPECT_TRUE(mockFatalWasCalled());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "This message is fatal"))
      << "\ncontent: [[" << mockFatalMessage() << "]]";
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "FATAL"));

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(file_content, "This message is fatal"));
   EXPECT_TRUE(verifyContent(file_content, "FATAL"));
   EXPECT_TRUE(verifyContent(file_content, "EXIT trigger caused by "));
}
TEST(LogTest, LOGF_IF__FATAL) {
   RestoreFileLogger logger(log_directory);
   EXPECT_FALSE(mockFatalWasCalled());
   LOGF_IF(FATAL, (2 < 3), "This message %s be worse", "could");
   EXPECT_TRUE(mockFatalWasCalled());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by ")) << "\n"
                                                                             << mockFatalMessage();
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "FATAL"));
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "This message could be worse"));

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(file_content, "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "FATAL"));
   EXPECT_TRUE(verifyContent(file_content, "This message could be worse"));
}
TEST(LogTest, LOG_IF__FATAL) {
   RestoreFileLogger logger(log_directory);
   LOG_IF(WARNING, (0 != t_info.compare(t_info))) << "This message should NOT be written";
   EXPECT_FALSE(mockFatalWasCalled());
   LOG_IF(FATAL, (0 != t_info.compare(t_info2))) << "This message should throw. xyz ";
   EXPECT_TRUE(mockFatalWasCalled());

   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "FATAL"));
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "This message should throw. xyz "));

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(file_content, "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "FATAL"));
   EXPECT_TRUE(verifyContent(file_content, "This message should throw. xyz "));
}
TEST(LogTest, LOG_IF__FATAL__NO_THROW) {
   RestoreFileLogger logger(log_directory);
   LOG_IF(FATAL, (2 > 3)) << "This message%sshould NOT throw";
   ASSERT_FALSE(mockFatalWasCalled());
}

// CHECK_F
TEST(CheckTest, CHECK_F__thisWILL_PrintErrorMsg) {
   RestoreFileLogger logger(log_directory);
   EXPECT_TRUE(mockFatalMessage().empty());
   EXPECT_FALSE(mockFatalWasCalled());

   CHECK(1 == 2);
   EXPECT_FALSE(mockFatalMessage().empty());
   EXPECT_TRUE(mockFatalWasCalled());

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT")) << "**** " << mockFatalMessage();
}

TEST(CHECK_F_Test, CHECK_F__thisWILL_PrintErrorMsg) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This message is added to throw %s and %s";
   std::string arg1 = "message";
   std::string arg3 = "log";

   CHECK_F(1 >= 2, msg.c_str(), arg1.c_str(), arg3.c_str());
   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT"));
}

TEST(CHECK_Test, CHECK__thisWILL_PrintErrorMsg) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This message is added to throw message and log";
   CHECK(1 >= 2) << msg;

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT"));
   EXPECT_TRUE(verifyContent(file_content, msg));
}
TEST(CHECK, CHECK_ThatWontThrow) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This %s should never appear in the %s";
   std::string msg3 = "This message should never appear in the log";
   CHECK(1 == 1);
   CHECK_F(1 == 1, msg.c_str(), "message", "log");
   logger.reset();
   EXPECT_FALSE(mockFatalWasCalled());

   std::string file_content = readFileToText(logger.logFile());
   EXPECT_FALSE(verifyContent(file_content, msg3));
   EXPECT_FALSE(verifyContent(mockFatalMessage(), msg3));
}

TEST(CHECK, CHECK_runtimeError) {
   RestoreFileLogger logger(log_directory);

   g3::setFatalExitHandler([](g3::FatalMessagePtr msg) {
      throw std::runtime_error("fatal test handler");
   });

   class dynamic_int_array {
      std::unique_ptr<int[]> data_;
      const int size_;

     public:
      explicit dynamic_int_array(int size) :
          data_{std::make_unique<int[]>(size)},
          size_(size) {}

      int& at(int i) {
         CHECK(i < size_);

         // unreachable if i >= size_
         return data_[i];
      }
   };

   dynamic_int_array arr{3};

   EXPECT_THROW(arr.at(3) = 1, std::runtime_error);
}

// see also test_io -- AddNonFatal
TEST(CustomLogLevels, AddFatal) {
   RestoreFileLogger logger(log_directory);
   const LEVELS DEADLY{FATAL.value + 1, {"DEADLY"}};
   EXPECT_TRUE(g3::internal::wasFatal(DEADLY));
   g_fatal_counter.store(0);
   ASSERT_FALSE(mockFatalWasCalled());
   g3::setFatalPreLoggingHook(fatalCounter);
#ifdef G3_DYNAMIC_LOGGING
   g3::only_change_at_initialization::addLogLevel(DEADLY, true);
#endif
   // clang-format off
   LOG(DEADLY) << "Testing my own custom level"; auto line = __LINE__;
   // clang-format on
   logger.reset();
   ASSERT_TRUE(mockFatalWasCalled());
   EXPECT_EQ(size_t{1}, g_fatal_counter.load());

   std::string file_content = readFileToText(logger.logFile());
   std::string expected;
   expected += "DEADLY [test_fatal.cpp->" + std::string(G3LOG_PRETTY_FUNCTION) + ":" + std::to_string(line);
   EXPECT_TRUE(verifyContent(file_content, expected)) << file_content
                                                      << "\n\nExpected: \n"
                                                      << expected;
   g_fatal_counter.store(0);  // restore
}
