///** ==========================================================================
// * 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
// * with no warranties. This code is yours to share, use and modify with no
// * strings attached and no restrictions or obligations.
// * ============================================================================*/
//
//
//#include <gtest/gtest.h>
//#include <memory>
//#include <fstream>
//#include <string>
//#include <memory>
//#include <future>
//#include <queue>
//
//
//#include <thread>
//#include "g2log.h"
//#include "g2logworker.h"
//#include "testing_helpers.h"
//
//using namespace testing_helper__cleaner;
//
//
//namespace { // anonymous
//  const char* name_path_1 = "./some_fake_DirectoryOrName_1_";
//  const char* name_path_2 = "./some_fake_DirectoryOrName_3_";
//  g2LogWorker* g_logger_ptr = nullptr;
//  LogFileCleaner* g_cleaner_ptr = nullptr;
//  
//  
//  
//  bool isTextAvailableInContent(const std::string &total_text, std::string msg_to_find) {
//    std::string content(total_text);
//    size_t location = content.find(msg_to_find);
//    return (location != std::string::npos);
//  }
//
//  std::string readFileToText(std::string filename) {
//    std::ifstream in;
//    in.open(filename.c_str(), std::ios_base::in);
//    if (!in.is_open()) {
//      return ""; // error just return empty string - test will 'fault'
//    }
//    std::ostringstream oss;
//    oss << in.rdbuf();
//    std::string content(oss.str());
//    return content;
//  }
//
//  std::string changeDirectoryOrName(std::string new_file_to_create) {
//    static std::mutex m;
//    static int count;
//    std::string add_count;
//    std::lock_guard<std::mutex> lock(m);
//    {
//      add_count = std::to_string(++count) + "_";
//      auto new_log = g_logger_ptr->changeLogFile(new_file_to_create + add_count).get();
//      if (!new_log.empty()) g_cleaner_ptr->addLogToClean(new_log);
//      return new_log;
//    }
//    return add_count;
//  }
//} // anonymous
//
//
//
//
//
//
//// TODO: this must change. Initialization of this is done here! and not in a special test_main.cpp
////  which MAY be OK ... however it is also very redundant with test_io
//
//TEST(TestOf_GetFileName, Expecting_ValidLogFile) {
//
//  LOG(INFO) << "test_filechange, Retrieving file name: ";
//  ASSERT_NE(g_logger_ptr, nullptr);
//  std::future<std::string> f_get_old_name = g_logger_ptr->logFileName();
//  ASSERT_TRUE(f_get_old_name.valid());
//  ASSERT_FALSE(f_get_old_name.get().empty());
//}
//
//TEST(TestOf_ChangingLogFile, Expecting_NewLogFileUsed) {
//  auto old_log = g_logger_ptr->logFileName().get();
//  std::string name = changeDirectoryOrName(name_path_1);
//  auto new_log = g_logger_ptr->changeLogFile(name).get();
//}
//
//TEST(TestOf_ManyThreadsChangingLogFileName, Expecting_EqualNumberLogsCreated) {
//  auto old_log = g_logger_ptr->logFileName().get();
//  if (!old_log.empty()) g_cleaner_ptr->addLogToClean(old_log);
//
//  LOG(INFO) << "SoManyThreadsAllDoingChangeFileName";
//  std::vector<std::thread> threads;
//  auto max = 2;
//  auto size = g_cleaner_ptr->size();
//  for (auto count = 0; count < max; ++count) {
//    std::string drive = ((count % 2) == 0) ? "./_threadEven_" : "./_threaOdd_";
//    threads.push_back(std::thread(changeDirectoryOrName, drive));
//  }
//  for (auto& thread : threads)
//    thread.join();
//
//  // check that all logs were created
//  ASSERT_EQ(size + max, g_cleaner_ptr->size());
//}
//
//TEST(TestOf_IllegalLogFileName, Expecting_NoChangeToOriginalFileName) {
//  std::string original = g_logger_ptr->logFileName().get();
//  std::cerr << "Below WILL print 'FiLE ERROR'. This is part of the testing and perfectly OK" << std::endl;
//  std::cerr << "****" << std::endl;
//  std::future<std::string> perhaps_a_name = g_logger_ptr->changeLogFile("XY:/"); // does not exist
//  ASSERT_TRUE(perhaps_a_name.get().empty());
//  std::cerr << "****" << std::endl;
//  std::string post_illegal = g_logger_ptr->logFileName().get();
//  ASSERT_STREQ(original.c_str(), post_illegal.c_str());
//}
//
//int main(int argc, char *argv[]) {
//  LogFileCleaner cleaner;
//  g_cleaner_ptr = &cleaner;
//  int return_value = 1;
//
//  std::string last_log_file;
//  {
//    g2LogWorker logger("ReplaceLogFile", name_path_2);
//    testing::InitGoogleTest(&argc, argv);
//    g_logger_ptr = &logger; // ugly but fine for this test
//    g2::initializeLogging(g_logger_ptr);
//    cleaner.addLogToClean(g_logger_ptr->logFileName().get());
//    return_value = RUN_ALL_TESTS();
//    last_log_file = g_logger_ptr->logFileName().get();
//    //g2::shutDownLogging();
//  }
//  std::cout << "FINISHED WITH THE TESTING" << std::endl;
//  // cleaning up
//  cleaner.addLogToClean(last_log_file);
//  return return_value;
//}
