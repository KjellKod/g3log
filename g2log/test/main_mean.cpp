/* *************************************************
// * Filename: g2log-main_mean.cpp
// * Created: 2011 by Kjell Hedström
// *
// * PUBLIC DOMAIN and Not copy-writed
// * ********************************************* */

// through CMakeLists.txt   #define of GOOGLE_GLOG_PERFORMANCE and G2LOG_PERFORMANCE
#include "performance.h"
#include <thread>

#if defined(G2LOG_PERFORMANCE)
const std::string title = "G2LOG";
#elif defined(GOOGLE_GLOG_PERFORMANCE)
const std::string title = "GOOGLE__GLOG";
#else
const std::string title = not_defined_this_will_be_compiler_error;
#endif

const std::string  g_prefix_log_name = title + "-performance-MEAN_LOG";
const std::string  g_measurement_dump= "/tmp/" + g_prefix_log_name + "_RESULT.txt";
const std::string g_path = "/tmp/";
using namespace g2_test;


int main(int argc, char** argv)
{
  std::ostringstream oss;
  oss << "\n\n" << title << " performance MEAN times\n";
  oss << "Running #: " << g_loop << " * " << g_iterations << " iterations of log entries" << std::endl;  // worst mean case is about 10us per log entry
  const size_t xtra_margin = 2;
  oss << "*** It can take som time. Please wait: Approximate maximum wait time is:  " << (long long) (g_iterations * 10 * xtra_margin / 1000000 ) << " seconds" << std::endl;
  writeTextToFile(g_measurement_dump, oss.str(), kAppend);
  oss.str(""); // clear the stream

#if defined(G2LOG_PERFORMANCE)
  LogWorker* logger = new LogWorker(g_prefix_log_name, g_path);
  g2::initializeLogging(logger);
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  google::InitGoogleLogging(argv[0]);
#endif

  auto start_time = std::chrono::steady_clock::now();
  doLogWrites(title);
  auto application_end_time = std::chrono::steady_clock::now();

#if defined(G2LOG_PERFORMANCE)
  delete logger; // will flush anything in the queue to file
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  google::ShutdownGoogleLogging();
#endif

  auto worker_end_time = std::chrono::steady_clock::now();
  auto application_time_us = std::chrono::duration_cast<microsecond>(application_end_time - start_time).count();
  auto total_time_us = std::chrono::duration_cast<microsecond>(worker_end_time - start_time).count();

  oss << "\n" << g_iterations << " log entries took: [" << total_time_us / 1000000 << " s] to write to disk"<< std::endl;
  oss << "[Application: \t\t\t:" << application_time_us/1000 << " ms]" << std::endl;
  oss << "[Background thread to finish\t:" << total_time_us/1000 << " ms]" << std::endl;
  oss << "\nAverage time per log entry:" << std::endl;
  oss << "[Application: " << application_time_us/g_iterations << " us]" << std::endl;
  oss << "[Background+Application: " << total_time_us/g_iterations << " us]" << std::endl;
  writeTextToFile(g_measurement_dump,oss.str(), kAppend);
  std::cout << "Result can be found at:" << g_measurement_dump << std::endl;

  return 0;
}
