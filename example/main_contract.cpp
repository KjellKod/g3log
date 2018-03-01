/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
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

   auto worker = g3::LogWorker::createLogWorker();
   auto handle= worker->addDefaultLogger(argv[0], path_to_log_file);
   g3::initializeLogging(worker.get());
   std::future<std::string> log_file_name = handle->call(&g3::FileSink::fileName);

   // Exmple of overriding the default formatting of log entry
   auto changeFormatting = handle->call(&g3::FileSink::overrideLogDetails, g3::LogMessage::FullLogDetailsToString);
   const std::string newHeader = "\t\tLOG format: [YYYY/MM/DD hh:mm:ss uuu* LEVEL THREAD_ID FILE->FUNCTION:LINE] message\n\t\t(uuu*: microseconds fractions of the seconds value)\n\n";
   // example of ovrriding the default formatting of header
   auto changeHeader = handle->call(&g3::FileSink::overrideLogHeader, newHeader);

   changeFormatting.wait();
   changeHeader.wait();


   std::cout << "*   This is an example of g3log. It WILL exit by a failed CHECK(...)" << std::endl;
   std::cout << "*   that acts as a FATAL trigger. Please see the generated log and " << std::endl;
   std::cout << "*   compare to the code at:\n*  \t g3log/test_example/main_contract.cpp" << std::endl;
   std::cout << "*\n*   Log file: [" << log_file_name.get() << "]\n\n" << std::endl;

   LOGF(INFO, "Hi log %d", 123);
   LOG(INFO) << "Test SLOG INFO";
   LOG(G3LOG_DEBUG) << "Test SLOG DEBUG";
   LOG(INFO) << "one: " << 1;
   LOG(INFO) << "two: " << 2;
   LOG(INFO) << "one and two: " << 1 << " and " << 2;
   LOG(G3LOG_DEBUG) << "float 2.14: " << 1000 / 2.14f;
   LOG(G3LOG_DEBUG) << "pi double: " << pi_d;
   LOG(G3LOG_DEBUG) << "pi float: " << pi_f;
   LOG(G3LOG_DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
   LOGF(INFO, "pi float printf:%f", pi_f);

   // FATAL SECTION
   int smaller = 1;
   int larger = 2;
   example_fatal::killWithContractIfNonEqual(smaller, larger);
}

