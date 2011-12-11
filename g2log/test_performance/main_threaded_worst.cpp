/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

// through CMakeLists.txt   #define of GOOGLE_GLOG_PERFORMANCE and G2LOG_PERFORMANCE
#include "performance.h"

#include <thread>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#if defined(G2LOG_PERFORMANCE)
const std::string title = "G2LOG";
#elif defined(GOOGLE_GLOG_PERFORMANCE)
const std::string title = "GOOGLE__GLOG";
#else
#error G2LOG_PERFORMANCE or GOOGLE_GLOG_PERFORMANCE was not defined
#endif


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	const std::string g_path = "./";
#else
	const std::string g_path = "/tmp/";
#endif



using namespace g2_test;


//
// OK: The code below isn't pretty but it works. Lots and lots of log entries
// to keep track of!
//
int main(int argc, char** argv)
{
  size_t number_of_threads =0;
  if(argc == 2)
  {
     number_of_threads = atoi(argv[1]);
  }
  if(argc != 2 || number_of_threads == 0)
  {
    std::cerr << "USAGE is: " << argv[0] << " number_threads" << std::endl;
    return 1;
  }
  std::ostringstream thread_count_oss;
  thread_count_oss << number_of_threads;

  const std::string  g_prefix_log_name = title + "-performance-" + thread_count_oss.str() + "threads-WORST_LOG";
  const std::string  g_measurement_dump= g_path + g_prefix_log_name + "_RESULT.txt";
  const std::string  g_measurement_bucket_dump= g_path + g_prefix_log_name + "_RESULT_buckets.txt";




  std::ostringstream oss;
  oss << "\n\n" << title << " performance " << number_of_threads << " threads WORST (PEAK) times\n";
  oss << "Each thread running #: " << g_loop << " * " << g_iterations << " iterations of log entries" << std::endl;  // worst mean case is about 10us per log entry
  const size_t xtra_margin = 2;
  oss << "*** It can take som time. Please wait: Approximate wait time on MY PC was:  " << number_of_threads * (long long) (g_iterations * 10 * xtra_margin / 1000000 ) << " seconds" << std::endl;
  writeTextToFile(g_measurement_dump, oss.str(), kAppend);
  oss.str(""); // clear the stream

#if defined(G2LOG_PERFORMANCE)
  g2LogWorker* logger = new g2LogWorker(g_prefix_log_name, g_path);
  g2::initializeLogging(logger);
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  google::InitGoogleLogging(argv[0]);
#endif

  std::thread* threads = new std::thread[number_of_threads];
  std::vector<long long>* threads_result = new std::vector<long long>[number_of_threads];

  // kiss: just loop, create threads, store them then join
  // could probably do this more elegant with lambdas
  for(size_t idx = 0; idx < number_of_threads; ++idx)
  {
    threads_result[idx].reserve(g_iterations);
  }

  auto start_time = std::chrono::steady_clock::now();
  for(size_t idx = 0; idx < number_of_threads; ++idx)
  {
    std::ostringstream count;
    count << idx+1;
    std::string thread_name =  title + "_T" + count.str();
    std::cout << "Creating thread: " << thread_name << std::endl;
    threads[idx] = std::thread(measurePeakDuringLogWrites,thread_name, std::ref(threads_result[idx]));
  }
  // wait for thread finishing
  for(size_t idx = 0; idx < number_of_threads; ++idx)
  {
    threads[idx].join();
  }
  auto application_end_time = std::chrono::steady_clock::now();
  delete [] threads;


#if defined(G2LOG_PERFORMANCE)
  delete logger; // will flush anything in the queue to file
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  google::ShutdownGoogleLogging();
#endif

  auto worker_end_time = std::chrono::steady_clock::now();
  auto application_time_us = std::chrono::duration_cast<microsecond>(application_end_time - start_time).count();
  auto total_time_us = std::chrono::duration_cast<microsecond>(worker_end_time - start_time).count();

  oss << "\n" << g_iterations << " log entries took: [" << total_time_us / 1000000 << " s] to write to disk"<< std::endl;
  oss << "[Application(" << number_of_threads << "_threads+overhead time for measurement):\t" << application_time_us/1000 << " ms]" << std::endl;
  oss << "[Background thread to finish:\t\t\t\t" << total_time_us/1000 << " ms]" << std::endl;
  oss << "\nAverage time per log entry:" << std::endl;
  oss << "[Application: " << application_time_us/(number_of_threads*g_iterations) << " us]" << std::endl;

  for(size_t idx = 0; idx < number_of_threads; ++idx)
  {
    std::vector<long long>& t_result = threads_result[idx];
    oss << "[Application t" << idx+1 << " worst took: " <<  (*std::max_element(t_result.begin(), t_result.end())) / 1000 << " ms]" << std::endl;
  }
  writeTextToFile(g_measurement_dump,oss.str(), kAppend);
  std::cout << "Result can be found at:" << g_measurement_dump << std::endl;

  // now split the result in buckets of 10ms each so that it's obvious how the peaks go
  std::vector<long long> all_measurements;
  all_measurements.reserve(g_iterations * number_of_threads);
  for(size_t idx = 0; idx < number_of_threads; ++idx)
  {
    std::vector<long long>& t_result = threads_result[idx];
    all_measurements.insert(all_measurements.end(), t_result.begin(), t_result.end());
  }
  delete [] threads_result; // finally get rid of them

  std::sort (all_measurements.begin(), all_measurements.end());
  std::map<long long, long long> value_amounts;
  // for(long long& idx : all_measurements) --- didn't work?!
  for(auto iter = all_measurements.begin(); iter != all_measurements.end(); ++iter)
  {
    auto value = (*iter)/1000; // convert to ms
    //auto bucket=floor(value*10+0.5)/10;
    ++value_amounts[value]; // asuming size_t is default 0 when initialized
  }

  oss.str("");
  oss << "Number of values rounted to milliseconds and put to [millisecond bucket] were dumped to file: " << g_measurement_bucket_dump << std::endl;
  oss << "Format:  bucket_of_ms, number_of_values_in_bucket\n\n" << std::endl;
  std::cout << oss.str() << std::endl;

  for(auto iter = value_amounts.begin(); iter != value_amounts.end(); ++iter)
  {
    oss << iter->first << "\t, " << iter->second << std::endl;
  }
  writeTextToFile(g_measurement_bucket_dump,oss.str(), kAppend,  false);


  return 0;
}
