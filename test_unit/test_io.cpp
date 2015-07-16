/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include <gtest/gtest.h>
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include "testing_helpers.h"
#include "g3log/loglevels.hpp"

#include <memory>
#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include <exception>
namespace {
const std::string log_directory = "./";
const std::string t_info = "test INFO ";
const std::string t_info2 = "test INFO 123";
const std::string t_debug = "test DEBUG ";
const std::string t_debug2 = "test DEBUG 1.123456";
const std::string t_warning = "test WARNING ";
const std::string t_warning2 = "test WARNING yello";

std::atomic<size_t> g_fatal_counter = {0};
void fatalCounter() {
   ++g_fatal_counter;
}

} // end anonymous namespace


using namespace testing_helpers;


/// THIS MUST BE THE FIRST UNIT TEST TO RUN! If any unit test run before this 
/// one then it could fail. For dynamic levels all levels are turned on only AT
/// instantiation so we do different test for dynamic logging levels
///
/// TODO : (gtest issue)
///Move out to separate unit test binary to ensure reordering of tests does not happen 
#ifdef G2_DYNAMIC_LOGGING
TEST(Initialization, No_Logger_Initialized___LevelsAreONByDefault) {
   EXPECT_FALSE(g2::internal::isLoggingInitialized());
   EXPECT_TRUE(g2::logLevel(DEBUG));
   EXPECT_TRUE(g2::logLevel(INFO));
   EXPECT_TRUE(g2::logLevel(WARNING));
   EXPECT_TRUE(g2::logLevel(FATAL));
   EXPECT_EQ(DEBUG.value, 0);
   EXPECT_EQ(INFO.value, 1);
   EXPECT_EQ(WARNING.value, 2);
   EXPECT_EQ(FATAL.value, 3);
}

TEST(Initialization, No_Logger_Initialized___Expecting_LOG_calls_to_be_Still_OKish) {
   EXPECT_FALSE(g2::internal::isLoggingInitialized());
   EXPECT_TRUE(g2::logLevel(INFO));
   EXPECT_TRUE(g2::logLevel(FATAL));
   EXPECT_TRUE(g2::logLevel(DEBUG));
   EXPECT_TRUE(g2::logLevel(WARNING));
   std::string err_msg1 = "Hey. I am not instantiated but I still should not crash. (I am g2logger)";
   std::string err_msg2_ignored = "This uninitialized message should be ignored";
   try {
      LOG(INFO) << err_msg1; // nothing happened. level not ON
      LOG(INFO) << err_msg2_ignored; // nothing happened. level not ON

   } catch (std::exception& e) {
      ADD_FAILURE() << "Should never have thrown even if it is not instantiated. Ignored exception:  " << e.what();
   }

   RestoreFileLogger logger(log_directory); // now instantiate the logger

   std::string good_msg1 = "This message could have pulled in the uninitialized_call message";
   LOG(INFO) << good_msg1;
   auto content = logger.resetAndRetrieveContent(); // this synchronizes with the LOG(INFO) call if debug level would be ON.
   ASSERT_TRUE(verifyContent(content, err_msg1)) << "Content: [" << content << "]";
   ASSERT_FALSE(verifyContent(content, err_msg2_ignored)) << "Content: [" << content << "]";
   ASSERT_TRUE(verifyContent(content, good_msg1)) << "Content: [" << content << "]";       
}
#else
TEST(Initialization, No_Logger_Initialized___Expecting_LOG_calls_to_be_Still_OKish) {
   EXPECT_FALSE(g2::internal::isLoggingInitialized());
   EXPECT_TRUE(g2::logLevel(INFO));
   EXPECT_TRUE(g2::logLevel(FATAL));
   EXPECT_TRUE(g2::logLevel(DEBUG));
   EXPECT_TRUE(g2::logLevel(WARNING));
   std::string err_msg1 = "Hey. I am not instantiated but I still should not crash. (I am g2logger)";
   std::string err_msg2_ignored = "This uninitialized message should be ignored";

   try {
      LOG(INFO) << err_msg1;
      LOG(INFO) << err_msg2_ignored;

   } catch (std::exception& e) {
      ADD_FAILURE() << "Should never have thrown even if it is not instantiated: " << e.what();
   }

   RestoreFileLogger logger(log_directory); // now instantiate the logger

   std::string good_msg1 = "This message will pull in also the uninitialized_call message";
   LOG(INFO) << good_msg1;
   auto content = logger.resetAndRetrieveContent(); // this synchronizes with the LOG(INFO) call.
   ASSERT_TRUE(verifyContent(content, err_msg1)) << "Content: [" << content << "]";
   ASSERT_FALSE(verifyContent(content, err_msg2_ignored)) << "Content: [" << content << "]";
   ASSERT_TRUE(verifyContent(content, good_msg1)) << "Content: [" << content << "]";
 }
#endif // #ifdef G2_DYNAMIC_LOGGING


TEST(Basics, Shutdown) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG(INFO) << "Not yet shutdown. This message should make it";
      logger.reset(); // force flush of logger (which will trigger a shutdown)
      LOG(INFO) << "Logger is shutdown,. this message will not make it (but it's safe to try)";
      file_content = readFileToText(logger.logFile()); // logger is already reset
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, "Not yet shutdown. This message should make it"));
   EXPECT_FALSE(verifyContent(file_content, "Logger is shutdown,. this message will not make it (but it's safe to try)"));
}

TEST(Basics, Shutdownx2) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG(INFO) << "Not yet shutdown. This message should make it";
      logger.reset(); // force flush of logger (which will trigger a shutdown)
      g2::internal::shutDownLogging(); // already called in reset, but safe to call again
      LOG(INFO) << "Logger is shutdown,. this message will not make it (but it's safe to try)";
      file_content = readFileToText(logger.logFile()); // already reset
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, "Not yet shutdown. This message should make it"));
   EXPECT_FALSE(verifyContent(file_content, "Logger is shutdown,. this message will not make it (but it's safe to try)"));
}

TEST(Basics, ShutdownActiveLogger) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG(INFO) << "Not yet shutdown. This message should make it";
      EXPECT_TRUE(g2::internal::shutDownLoggingForActiveOnly(logger._scope->get()));
      LOG(INFO) << "Logger is shutdown,. this message will not make it (but it's safe to try)";
      file_content = logger.resetAndRetrieveContent();
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, "Not yet shutdown. This message should make it")) << "\n\n\n***************************\n" << file_content;
   EXPECT_FALSE(verifyContent(file_content, "Logger is shutdown,. this message will not make it (but it's safe to try)"));
}

TEST(Basics, DoNotShutdownActiveLogger) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG(INFO) << "Not yet shutdown. This message should make it";
      std::unique_ptr<g2::LogWorker> duplicateLogWorker{g2::LogWorker::createWithNoSink()};
      EXPECT_FALSE(g2::internal::shutDownLoggingForActiveOnly(duplicateLogWorker.get()));
      LOG(INFO) << "Logger is (NOT) shutdown,. this message WILL make it";
      file_content = logger.resetAndRetrieveContent();
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, "Not yet shutdown. This message should make it"));
   EXPECT_TRUE(verifyContent(file_content, "Logger is (NOT) shutdown,. this message WILL make it")) << file_content;
}


TEST(LOGTest, LOG) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      EXPECT_TRUE(g2::logLevel(INFO));
      EXPECT_TRUE(g2::logLevel(FATAL));
      LOG(INFO) << "test LOG(INFO)";
      logger.reset(); // force flush of logger
      file_content = readFileToText(logger.logFile());
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, "test LOG(INFO)"));
   EXPECT_TRUE(g2::logLevel(INFO));
   EXPECT_TRUE(g2::logLevel(FATAL));
}



// printf-type log


TEST(LogTest, LOG_F) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      std::cout << "logfilename: " << logger.logFile() << std::flush << std::endl;

      LOGF(INFO, std::string(t_info + "%d").c_str(), 123);
      LOGF(DEBUG, std::string(t_debug + "%f").c_str(), 1.123456);
      LOGF(WARNING, std::string(t_warning + "%s").c_str(), "yello");
      logger.reset(); // force flush of logger
      file_content = readFileToText(logger.logFile());
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   ASSERT_TRUE(verifyContent(file_content, t_info2));
   ASSERT_TRUE(verifyContent(file_content, t_debug2));
   ASSERT_TRUE(verifyContent(file_content, t_warning2));
}




// stream-type log
TEST(LogTest, LOG) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG(INFO) << t_info << 123;
      LOG(DEBUG) << t_debug << std::setprecision(7) << 1.123456f;
      LOG(WARNING) << t_warning << "yello";
      logger.reset(); // force flush of logger
      file_content = readFileToText(logger.logFile());
      SCOPED_TRACE("LOG_INFO"); // Scope exit be prepared for destructor failure
   }
   ASSERT_TRUE(verifyContent(file_content, t_info2));
   ASSERT_TRUE(verifyContent(file_content, t_debug2));
   ASSERT_TRUE(verifyContent(file_content, t_warning2));
}


TEST(LogTest, LOG_F_IF) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOGF_IF(INFO, (2 == 2), std::string(t_info + "%d").c_str(), 123);
      LOGF_IF(DEBUG, (2 != 2), std::string(t_debug + "%f").c_str(), 1.123456);
      logger.reset(); // force flush of logger
      file_content = readFileToText(logger.logFile());
      SCOPED_TRACE("LOG_IF"); // Scope exit be prepared for destructor failure
   }
   ASSERT_TRUE(verifyContent(file_content, t_info2));
   ASSERT_FALSE(verifyContent(file_content, t_debug2));
}


TEST(LogTest, LOG_IF) {
   std::string file_content;
   {
      RestoreFileLogger logger(log_directory);
      LOG_IF(INFO, (2 == 2)) << t_info << 123;
      LOG_IF(DEBUG, (2 != 2)) << t_debug << std::setprecision(7) << 1.123456f;
      logger.reset(); // force flush of logger
      file_content = readFileToText(logger.logFile());
      SCOPED_TRACE("LOG_IF"); // Scope exit be prepared for destructor failure
   }
   EXPECT_TRUE(verifyContent(file_content, t_info2));
   EXPECT_FALSE(verifyContent(file_content, t_debug2));
}
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


TEST(LogTest, LOG_preFatalLogging_hook) {
   {
      RestoreFileLogger logger(log_directory);
      ASSERT_FALSE(mockFatalWasCalled());
      g_fatal_counter.store(0);
      g2::setFatalPreLoggingHook(fatalCounter);   
      LOG(FATAL) << "This message is fatal";
      logger.reset();
      EXPECT_EQ(g_fatal_counter.load(), size_t{1});
   }
   {  // Now with no fatal pre-logging-hook
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
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by ")) << "\n" << mockFatalMessage();
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
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT"))  << "**** " << mockFatalMessage();
}


TEST(CHECK_F_Test, CHECK_F__thisWILL_PrintErrorMsg) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This message is added to throw %s and %s";
   std::string msg2 = "This message is added to throw message and log";
   std::string arg1 = "message";
   std::string arg2 = "log";

   CHECK_F(1 >= 2, msg.c_str(), arg1.c_str(), arg2.c_str());
   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT"));
}


TEST(CHECK_Test, CHECK__thisWILL_PrintErrorMsg) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This message is added to throw %s and %s";
   std::string msg2 = "This message is added to throw message and log";
   std::string arg1 = "message";
   std::string arg2 = "log";
   CHECK(1 >= 2) << msg2;

   logger.reset();
   std::string file_content = readFileToText(logger.logFile());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), "EXIT trigger caused by "));
   EXPECT_TRUE(verifyContent(file_content, "CONTRACT"));
   EXPECT_TRUE(verifyContent(file_content, msg2));
}
TEST(CHECK, CHECK_ThatWontThrow) {
   RestoreFileLogger logger(log_directory);
   std::string msg = "This %s should never appear in the %s";
   std::string msg2 = "This message should never appear in the log";
   std::string arg1 = "message";
   std::string arg2 = "log";

   CHECK(1 == 1);
   CHECK_F(1 == 1, msg.c_str(), "message", "log");
   logger.reset();
   EXPECT_FALSE(mockFatalWasCalled());

   std::string file_content = readFileToText(logger.logFile());
   EXPECT_FALSE(verifyContent(file_content, msg2));
   EXPECT_FALSE(verifyContent(mockFatalMessage(), msg2));
}





#ifdef G2_DYNAMIC_LOGGING 
namespace {
   // Restore dynamic levels if turned off

   struct RestoreDynamicLoggingLevels {
      RestoreDynamicLoggingLevels() {
      };
      ~RestoreDynamicLoggingLevels() {
         g2::setLogLevel(DEBUG, false);
         g2::setLogLevel(INFO, false);
         g2::setLogLevel(WARNING, false);
         g2::setLogLevel(FATAL, false);
      }
   };
} // anonymous
TEST(DynamicLogging, DynamicLogging_IS_ENABLED) {
   RestoreDynamicLoggingLevels raiiLevelRestore;

   ASSERT_TRUE(g2::logLevel(DEBUG));
   ASSERT_TRUE(g2::logLevel(INFO));
   ASSERT_TRUE(g2::logLevel(WARNING));
   ASSERT_TRUE(g2::logLevel(FATAL)); // Yes FATAL can be turned off. Thereby rendering it ineffective.
   g2::setLogLevel(DEBUG, false);
   ASSERT_FALSE(g2::logLevel(DEBUG));
   ASSERT_TRUE(g2::logLevel(INFO));
   ASSERT_TRUE(g2::logLevel(WARNING));
   ASSERT_TRUE(g2::logLevel(FATAL)); // Yes FATAL can be turned off. Thereby rendering it ineffective.

   g2::setLogLevel(INFO, false);
   ASSERT_FALSE(g2::logLevel(DEBUG));
   ASSERT_FALSE(g2::logLevel(INFO));
   ASSERT_TRUE(g2::logLevel(WARNING));
   ASSERT_TRUE(g2::logLevel(FATAL)); // Yes FATAL can be turned off. Thereby rendering it ineffective.

   g2::setLogLevel(WARNING, false);
   ASSERT_FALSE(g2::logLevel(DEBUG));
   ASSERT_FALSE(g2::logLevel(INFO));
   ASSERT_FALSE(g2::logLevel(WARNING));
   ASSERT_TRUE(g2::logLevel(FATAL)); // Yes FATAL can be turned off. Thereby rendering it ineffective.

   g2::setLogLevel(FATAL, false);
   ASSERT_FALSE(g2::logLevel(DEBUG));
   ASSERT_FALSE(g2::logLevel(INFO));
   ASSERT_FALSE(g2::logLevel(WARNING));
   ASSERT_FALSE(g2::logLevel(FATAL)); // Yes FATAL can be turned off. Thereby rendering it ineffective.
}
TEST(DynamicLogging, DynamicLogging_No_Logs_If_Disabled) {
   {
      RestoreFileLogger logger(log_directory);   
      ASSERT_TRUE(g2::logLevel(DEBUG));
      ASSERT_TRUE(g2::logLevel(INFO));
      ASSERT_TRUE(g2::logLevel(WARNING));
      ASSERT_TRUE(g2::logLevel(FATAL));
   }

   RestoreDynamicLoggingLevels raiiLevelRestore;
   
   std::string msg_debugOn = "This %s SHOULD  appear in the %s";
   std::string msg_debugOff = "This message should never appear in the log";
   std::string msg_info1 = "This info msg log";
   try {
      {
              RestoreFileLogger logger(log_directory);   
              LOGF(DEBUG, msg_debugOn.c_str(), "msg", "log");
              auto content = logger.resetAndRetrieveContent();
              ASSERT_TRUE(verifyContent(content, "This msg SHOULD  appear in the log")) << "Content: [" << content << "]";
      }

      {
         RestoreFileLogger logger(log_directory);
         g2::setLogLevel(DEBUG, false);
         EXPECT_FALSE(g2::logLevel(DEBUG));
         LOG(DEBUG) << msg_debugOff;
         auto content = logger.resetAndRetrieveContent();
         ASSERT_FALSE(verifyContent(content, "This message should never appear in the log")) << "Content: [" << content << "]";
      }
      
   } catch (std::exception const &e) {
      std::cerr << e.what() << std::endl;
      ADD_FAILURE() << "Should never have thrown";
   }
}
TEST(DynamicLogging, DynamicLogging_No_Fatal_If_Disabled) {
   RestoreFileLogger logger(log_directory);
   RestoreDynamicLoggingLevels raiiLevelRestore;
   ASSERT_TRUE(g2::logLevel(DEBUG));
   ASSERT_TRUE(g2::logLevel(INFO));
   ASSERT_TRUE(g2::logLevel(WARNING));
   ASSERT_TRUE(g2::logLevel(FATAL));

   std::string msg1 = "This IS fatal (not crash, since it is unit test";

   LOG(FATAL) << msg1;
   EXPECT_TRUE(mockFatalWasCalled());
   EXPECT_FALSE(mockFatalMessage().empty());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), msg1));
   
   clearMockFatal();
   EXPECT_FALSE(mockFatalWasCalled());
   
   
   g2::setLogLevel(FATAL, false);
   std::string msg2 = "This is NOT fatal (not crash, since it is unit test. FATAL is disabled";
   LOG(FATAL) << msg2;
   EXPECT_FALSE(mockFatalWasCalled());
   EXPECT_TRUE(mockFatalMessage().empty());
}


TEST(DynamicLogging, DynamicLogging_Check_WillAlsoBeTurnedOffWhen_Fatal_Is_Disabled) {
   RestoreFileLogger logger(log_directory);
   RestoreDynamicLoggingLevels raiiLevelRestore;
   ASSERT_TRUE(g2::logLevel(FATAL));

   std::string msg1 = "dummy message to check if CHECK worked when fatal is enabled";
   std::string msg2 = "dummy message to check if CHECK worked when fatal is disabled";
   LOG(FATAL) << msg1;
   EXPECT_TRUE(mockFatalWasCalled());
   EXPECT_TRUE(verifyContent(mockFatalMessage(), msg1));
   
   clearMockFatal();
   EXPECT_FALSE(mockFatalWasCalled());
 
   // Disable also CHECK calls
   g2::setLogLevel(FATAL, false);
   ASSERT_FALSE(g2::logLevel(FATAL));
   LOG(FATAL) << msg2;
   EXPECT_FALSE(mockFatalWasCalled());
}



#else 
TEST(DynamicLogging, DynamicLogging_IS_NOT_ENABLED) {
   ASSERT_TRUE(g2::logLevel(DEBUG));
   //g2::setLogLevel(DEBUG, false);  this line will not compile since G2_DYNAMIC_LOGGING is not enabled. Kept for show.
   //ASSERT_FALSE(g2::logLevel(DEBUG));
}
#endif // Dynamic logging



