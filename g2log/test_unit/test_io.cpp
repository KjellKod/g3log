/* *************************************************
 * Filename: test_io.cpp
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copy-writed
 * ********************************************* */

#include <gtest/gtest.h>
#include "g2log.h"
#include "g2logworker.h"
#include <memory>
#include <string>
#include <fstream>
#include <cstdio>


namespace
{
const int k_wait_time = 5; // 5s wait between LOG/CHECK FATAL till we say it's too long time

bool verifyContent(const std::string &total_text,std::string msg_to_find)
{
  std::string content(total_text);
  size_t location = content.find(msg_to_find);
  return (location != std::string::npos);
}

std::string readFileToText(std::string filename)
{
  std::ifstream in;
  in.open(filename.c_str(),std::ios_base::in);
  if(!in.is_open())
  {
    return ""; // error just return empty string - test will 'fault'
  }
  std::ostringstream oss;
  oss << in.rdbuf();
  std::string content(oss.str());
  return content;
}
} // end anonymous namespace

// RAII temporarily replace of logger
// and restoration of original logger at scope end
struct RestoreLogger
{
  RestoreLogger();
  ~RestoreLogger();

  void reset();
  std::string readFileToText();

  std::unique_ptr<g2LogWorker> logger_;
  const std::string log_file_;

};

RestoreLogger::RestoreLogger()
  : logger_(new g2LogWorker("UNIT_TEST_LOGGER", "./"))
  , log_file_(logger_->logFileName())
{
  g2::initializeLogging(logger_.get());
  g2::internal::changeFatalInitHandlerForUnitTesting();
}

RestoreLogger::~RestoreLogger()
{
  reset();
  g2::shutDownLogging();
  EXPECT_EQ(0, remove(log_file_.c_str()));
}

void RestoreLogger::reset()
{
  logger_.reset();
}




// LOG
TEST(LOGTest, LOG)
{
  std::string file_content;
  {
    RestoreLogger logger;
    LOG(INFO) << "test LOG(INFO)";
    logger.reset(); // force flush of logger
    file_content = readFileToText(logger.log_file_);
    SCOPED_TRACE("LOG_INFO");  // Scope exit be prepared for destructor failure
  }
  ASSERT_TRUE(verifyContent(file_content, "test LOG(INFO)"));
}

namespace
{
const std::string t_info = "test INFO ";
const std::string t_info2 = "test INFO 123";
const std::string t_debug = "test DEBUG ";
const std::string t_debug2 = "test DEBUG 1.123456";
const std::string t_warning = "test WARNING ";
const std::string t_warning2 = "test WARNING yello";
}

// printf-type log
TEST(LogTest, LOG_F)
{

  std::string file_content;
  {
    RestoreLogger logger;
    LOGF(INFO, std::string(t_info + "%d").c_str(), 123);
    LOGF(DEBUG, std::string(t_debug + "%f").c_str(), 1.123456);
    LOGF(WARNING, std::string(t_warning + "%s").c_str(), "yello");
    logger.reset(); // force flush of logger
    file_content = readFileToText(logger.log_file_);
    SCOPED_TRACE("LOG_INFO");  // Scope exit be prepared for destructor failure
  }
  ASSERT_TRUE(verifyContent(file_content, t_info2));
  ASSERT_TRUE(verifyContent(file_content, t_debug2));
  ASSERT_TRUE(verifyContent(file_content, t_warning2));
}

// stream-type log
TEST(LogTest, LOG)
{
  std::string file_content;
  {
    RestoreLogger logger;
    LOG(INFO) << t_info   << 123;
    LOG(DEBUG) <<  t_debug  << std::setprecision(7) << 1.123456f;
    LOG(WARNING) << t_warning << "yello";
    logger.reset(); // force flush of logger
    file_content = readFileToText(logger.log_file_);
    SCOPED_TRACE("LOG_INFO");  // Scope exit be prepared for destructor failure
  }
  ASSERT_TRUE(verifyContent(file_content, t_info2));
  ASSERT_TRUE(verifyContent(file_content, t_debug2));
  ASSERT_TRUE(verifyContent(file_content, t_warning2));
}


TEST(LogTest, LOG_F_IF)
{
  std::string file_content;
  {
    RestoreLogger logger;
    LOGF_IF(INFO, (2 == 2), std::string(t_info + "%d").c_str(), 123);
    LOGF_IF(DEBUG, (2 != 2), std::string(t_debug + "%f").c_str(), 1.123456);
    logger.reset(); // force flush of logger
    file_content = readFileToText(logger.log_file_);
    SCOPED_TRACE("LOG_IF");  // Scope exit be prepared for destructor failure
  }
  ASSERT_TRUE(verifyContent(file_content, t_info2));
  ASSERT_FALSE(verifyContent(file_content, t_debug2));
}

TEST(LogTest, LOG_IF)
{
  std::string file_content;
  {
    RestoreLogger logger;
    LOG_IF(INFO, (2 == 2))  << t_info   << 123;
    LOG_IF(DEBUG, (2 != 2)) <<  t_debug  << std::setprecision(7) << 1.123456f;
    logger.reset(); // force flush of logger
    file_content = readFileToText(logger.log_file_);
    SCOPED_TRACE("LOG_IF");  // Scope exit be prepared for destructor failure
  }
  ASSERT_TRUE(verifyContent(file_content, t_info2));
  ASSERT_FALSE(verifyContent(file_content, t_debug2));
}

TEST(LogTest, LOGF__FATAL)
{
  RestoreLogger logger;
  try
  {
    LOGF(FATAL, "This message should throw %d",0);
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    std::cerr << file_content << std::endl << std::flush;
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, "This message should throw"))
    {
      SUCCEED();
      return;
    }
    else
    {
      ADD_FAILURE() << "Didn't throw exception as expected";
    }
  }
  ADD_FAILURE() << "Didn't throw exception at ALL";
}


TEST(LogTest, LOG_FATAL)
{
  RestoreLogger logger;
  try
  {
    LOG(FATAL) << "This message should throw";
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, "This message should throw"))
    {
      SUCCEED();
      return;
    }
    else
    {
      ADD_FAILURE() << "Didn't throw exception as expected";
    }
  }
  ADD_FAILURE() << "Didn't throw exception at ALL";
}


TEST(LogTest, LOGF_IF__FATAL)
{
  RestoreLogger logger;
  try
  {
    LOGF_IF(FATAL, (2<3), "This message%sshould throw"," ");
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, "This message should throw"))
    {
      SUCCEED();
      return;
    }
    else
    {
      ADD_FAILURE() << "Didn't throw exception as expected";
    }
  }
  ADD_FAILURE() << "Didn't throw exception at ALL";
}


TEST(LogTest, LOG_IF__FATAL)
{
  RestoreLogger logger;
  try
  {
    LOG_IF(WARNING, (0 != t_info.compare(t_info))) << "This message should NOT be written";
    LOG_IF(FATAL, (0 != t_info.compare(t_info2))) << "This message should throw";
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, "This message should throw") &&
       (false == verifyContent(file_content, "This message should NOT be written")))
    {
      SUCCEED();
      return;
    }
    else
    {
      ADD_FAILURE() << "Didn't throw exception as expected";
    }
  }
  ADD_FAILURE() << "Didn't throw exception at ALL";
}

TEST(LogTest, LOG_IF__FATAL__NO_THROW)
{
  RestoreLogger logger;
  try
  {
    LOG_IF(FATAL, (2>3)) << "This message%sshould NOT throw";
  }
  catch (std::exception const &e)
  {
    std::cerr << e.what() << std::endl;
    logger.reset();
    ADD_FAILURE() << "Didn't throw exception as expected";
  }
  logger.reset();
  SUCCEED();
}


// CHECK_F
TEST(CheckTest, CHECK_F__thisWILL_PrintErrorMsg)
{
  RestoreLogger logger;
  try
  {
    CHECK(1 == 2);
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL"))
    {
      SUCCEED();
      return;
    }
  }
  ADD_FAILURE() << "Didn't throw exception as expected";
}


TEST(CHECK_F_Test, CHECK_F__thisWILL_PrintErrorMsg)
{
  RestoreLogger logger;
  std::string msg = "This message is added to throw %s and %s";
  std::string msg2 = "This message is added to throw message and log";
  std::string arg1 = "message";
  std::string arg2 = "log";
  try
  {
    CHECK_F(1 >= 2, msg.c_str(), arg1.c_str(), arg2.c_str());
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, msg2))
    {
      SUCCEED();
      return;
    }
  }
  ADD_FAILURE() << "Didn't throw exception as expected";
}

TEST(CHECK_Test, CHECK__thisWILL_PrintErrorMsg)
{
  RestoreLogger logger;
  std::string msg = "This message is added to throw %s and %s";
  std::string msg2 = "This message is added to throw message and log";
  std::string arg1 = "message";
  std::string arg2 = "log";
  try
  {
    CHECK(1 >= 2) << msg2;
  }
  catch (std::exception const &e)
  {
    logger.reset();
    std::string file_content = readFileToText(logger.log_file_);
    if(verifyContent(e.what(), "EXIT trigger caused by ") &&
       verifyContent(file_content, "FATAL") &&
       verifyContent(file_content, msg2))
    {
      SUCCEED();
      return;
    }
  }
  ADD_FAILURE() << "Didn't throw exception as expected";
}


TEST(CHECK, CHECK_ThatWontThrow)
{
  RestoreLogger logger;
  std::string msg = "This %s should never appear in the %s";
  std::string msg2 = "This message should never appear in the log";
  std::string arg1 = "message";
  std::string arg2 = "log";
  try
  {
    CHECK(1 == 1);
    CHECK_F(1==1, msg.c_str(), "message", "log");
  }
  catch (std::exception const &e)
  {
    std::cerr << e.what() << std::endl;
    ADD_FAILURE() << "Should never have thrown";
  }

  std::string file_content = readFileToText(logger.log_file_);
  ASSERT_FALSE(verifyContent(file_content, msg2));
}

