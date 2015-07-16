/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g2log.hpp"
#include <iomanip>
#include <thread>
#include <iostream>


namespace
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   const std::string path_to_log_file = "./";
#else
   const std::string path_to_log_file = "/tmp/";
#endif
}

namespace example_fatal
{
   void killWithContractIfNonEqual(int first, int second)
   {
      CHECK(first == second) << "Test to see if contract works: onetwothree: " << 123 << ". This should be at the end of the log, and will exit this example";
   }
} // example fatal

int main(int argc, char **argv)
{
   double pi_d = 3.1415926535897932384626433832795;
   float pi_f = 3.1415926535897932384626433832795f;

   auto logger_n_handle = g2::LogWorker::createWithDefaultLogger(argv[0], path_to_log_file);
   g2::initializeLogging(logger_n_handle.worker.get());
   std::future<std::string> log_file_name = logger_n_handle.sink->call(&g2::FileSink::fileName);
   std::cout << "*   This is an example of g2log. It WILL exit by a failed CHECK(...)" << std::endl;
   std::cout << "*   that acts as a FATAL trigger. Please see the generated log and " << std::endl;
   std::cout << "*   compare to the code at:\n*  \t g2log/test_example/main_contract.cpp" << std::endl;
   std::cout << "*\n*   Log file: [" << log_file_name.get() << "]\n\n" << std::endl;

   LOGF(INFO, "Hi log %d", 123);
   LOG(INFO) << "Test SLOG INFO";
   LOG(DEBUG) << "Test SLOG DEBUG";
   LOG(INFO) << "one: " << 1;
   LOG(INFO) << "two: " << 2;
   LOG(INFO) << "one and two: " << 1 << " and " << 2;
   LOG(DEBUG) << "float 2.14: " << 1000 / 2.14f;
   LOG(DEBUG) << "pi double: " << pi_d;
   LOG(DEBUG) << "pi float: " << pi_f;
   LOG(DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
   LOGF(INFO, "pi float printf:%f", pi_f);

   // FATAL SECTION
   int smaller = 1;
   int larger = 2;
   example_fatal::killWithContractIfNonEqual(smaller, larger);
}

