
// Public domain @ref: Unlicense.org
// made by KjellKod, 2015. Feel free to share, modify etc with no obligations but also with no guarantees from my part either
// enjoy - Kjell Hedstrom (aka KjellKod)

#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <map>
#include <future>
#include "g2logworker.hpp"
#include "g2log.hpp"

namespace {
const uint64_t g_iterations{1000000};


std::atomic<size_t> g_counter = {0};
void MeasurePeakDuringLogWrites(const size_t id, std::vector<uint64_t>& result) {

   while (true) {
      const size_t value_now = ++g_counter;
      if (value_now > g_iterations) {
         return;
      }
      auto start_time = std::chrono::high_resolution_clock::now();
      LOG(INFO) << "Some text to log for thread: " << id;
      auto stop_time = std::chrono::high_resolution_clock::now();
      uint64_t time_us = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();
      result.push_back(time_us);

   }
}


void   PrintStats(const size_t number_of_threads,   const std::map<size_t, std::vector<uint64_t>>& threads_result, const uint64_t total_time_in_us) {

   size_t idx = 0;
   for (auto t_result: threads_result) {
      uint64_t worstUs = (*std::max_element(t_result.second.begin(), t_result.second.end()));
      std::cout << idx++ << " the worst thread latency was:" <<  worstUs / uint64_t(1000) << " ms  (" << worstUs << " us)] " << std::endl;
   }

   std::cout << "Total time :" << total_time_in_us / uint64_t(1000) << " ms (" << total_time_in_us
             << " us)" << std::endl;
   std::cout << "Average time: " << total_time_in_us / 1000000 << std::endl;
}



void SaveResultToBucketFile(std::string result_filename, const std::map<size_t, std::vector<uint64_t>>& threads_result) {
   // now split the result in buckets of 1ms each so that it's obvious how the peaks go
   std::vector<uint64_t> all_measurements;
   all_measurements.reserve(g_iterations);
   for (auto& t_result : threads_result) {
      all_measurements.insert(all_measurements.end(), t_result.second.begin(), t_result.second.end());
   }

   // doesn't matter but is fun in case someone wants to print them out
   // std::sort (all_measurements.begin(), all_measurements.end());

   // Add to buckets. Values are zero initialized
   // http://stackoverflow.com/questions/16177596/stdmapstring-int-default-initialization-of-value
   std::map<uint64_t, uint64_t> buckets;
   const uint64_t us_to_ms = 1000;
   for (auto value : all_measurements) {
      value = value / us_to_ms;
      ++buckets[value];
   }

   // save to readable format
   std::ostringstream oss;
   oss << "\n\n Millisecond bucket measurement" << std::endl;
   for (const auto ms_bucket : buckets) {
      oss << ms_bucket.first << "\t, " << ms_bucket.second << std::endl;
   }

   std::ofstream out;
   std::ios_base::openmode mode = std::ios_base::out | std::ios_base::trunc;
   ;
   out.open(result_filename.c_str(), mode);
   if (!out.is_open()) {
      std::cerr << "Error writing to " << result_filename << std::endl;
   }
   out << oss.str() << std::flush;
}

} // anonymous


// The purpose of this test is NOT to see how fast
// each thread can possibly write. It is to see what
// the worst latency is for writing a log entry
//
// In the test 1 million log entries will be written
// an atomic counter is used to give each thread what
// it is to write next. The overhead of atomic
// synchronization between the threads are not counted in the worst case latency
int main(int argc, char** argv) {
   size_t number_of_threads {0};
   if (argc == 2) {
      number_of_threads = atoi(argv[1]);
   }
   if (argc != 2 || number_of_threads == 0) {
      std::cerr << "USAGE is: " << argv[0] << " number_threads" << std::endl;
      return 1;
   }


   std::vector<std::thread> threads(number_of_threads);
   std::map<size_t, std::vector<uint64_t>> threads_result;

   for (auto idx = 0; idx < number_of_threads; ++idx) {
      // reserve to 1 million for all the result
      // it's a test so  let's not care about the wasted space
      threads_result[idx].reserve(g_iterations);
   }

   auto logger_n_handle = g2::LogWorker::createWithDefaultLogger(argv[0], "./");
   g2::initializeLogging(logger_n_handle.worker.get());
   std::future<std::string> log_file_name = logger_n_handle.sink->call(&g2::FileSink::fileName);
   auto filename = log_file_name.get();

   std::cout << "Using " << number_of_threads;
   std::cout << " to log in total 1 million log entries to " << filename << std::endl;


   auto start_time_application_total = std::chrono::high_resolution_clock::now();
   for (uint64_t idx = 0; idx < number_of_threads; ++idx) {
      threads[idx] = std::thread(MeasurePeakDuringLogWrites, idx, std::ref(threads_result[idx]));
   }
   for (auto idx = 0; idx < number_of_threads; ++idx) {
      threads[idx].join();
   }
   auto stop_time_application_total = std::chrono::high_resolution_clock::now();

   uint64_t total_time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(stop_time_application_total - start_time_application_total).count();

   PrintStats(number_of_threads, threads_result, total_time_in_us);
   SaveResultToBucketFile(filename + ".result.csv", threads_result);
   return 0;
}