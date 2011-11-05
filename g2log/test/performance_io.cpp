/* *************************************************
 * Filename: performance_io.cpp
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copy-writed
 * ********************************************* */


#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>

#include "loghelper.h"
#include "logworker.h"
#include "g2log.h"

//typedef std::chrono::duration<long, std::ratio<1, 1000> > millisecond;
typedef std::chrono::duration<long long, std::ratio<1, 1000000> > microsecond;
namespace
{
enum WriteMode
{
  kAppend = 0,
  kTruncate = 1
};

enum TestType
{
  kTypeSimpleStream, kTypeComplexStream, kTypeSimpleF, kTypeComplexF
};

const int k_loop = 1; //3;
const int k_iterations = 1000000; //1000000;
const std::string measure_g2log = "/tmp/g2log-measure_performance.txt";
const std::string  raw_log_name = "g2log-by-kjellkod"; // needed by the g2log instantiation
const std::string g_path = "/tmp/";
long long g_simple_max_us = 0;
long long g_complex_max_us = 0;
long long g_simpleF_max_us = 0;
long long g_complexF_max_us = 0;
const char* charptrmsg = "\tmessage by char*";
const std::string strmsg = "\tmessage by string";
float pi_f = 3.1415926535897932384626433832795f;


bool writeTextToFile(const std::string& filename, const std::string& msg, const WriteMode write_mode)
{
  std::cout << msg << std::flush;

  std::ofstream out;
  std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
  (kTruncate == write_mode) ? mode |= std::ios_base::trunc : mode |= std::ios_base::app;
  out.open(filename.c_str(), mode);
  if (!out.is_open())
  {
    std::ostringstream ss_error;
    ss_error << "Fatal error could not open log file:[" << filename << "]";
    ss_error << "\n\t\t std::ios_base state = " << out.rdstate();
    std::cerr << ss_error.str().c_str() << std::endl << std::flush;
    return false;
  }
  out << msg;
  return true;
}

long long mean(const std::vector<long long> &v)
{
  long long total =  std::accumulate(v.begin(), v.end(), 0 ); // '0' is the initial value
  return total/v.size();
}


long long iterativeLog(const TestType type,std::vector<long long> &measurements)
{
  auto start_total_time = std::chrono::system_clock::now();
  for(int i= 0; i < k_iterations; ++i)
  {
    auto maxmin_start_time = std::chrono::system_clock::now();
    if(kTypeSimpleStream == type)
    {
      LOG(INFO) << "#1 g2log STREAM";
    }
    else if(kTypeSimpleF == type)
    {
      LOG_F(INFO, "#2 g2log LOG_F simple %d",0);
    }
    else if(kTypeComplexStream == type)
    {
      LOG(INFO) << "#3 g2log STREAM, iteration #" << i << " " << charptrmsg << strmsg << " and a float: " << std::setprecision(6) << pi_f;
    }
    else // kTypeComplexF
    {
      LOG_F(INFO, "#4, iteration #%d %s%s and a float:%f ", i,charptrmsg,strmsg.c_str(),pi_f);
    }
    auto maxmin_stop_time = std::chrono::system_clock::now();
    auto maxmin_log_time = std::chrono::duration_cast<microsecond>(maxmin_stop_time - maxmin_start_time).count();
    measurements.push_back(maxmin_log_time);
  }
  auto now_total = std::chrono::system_clock::now();
  return std::chrono::duration_cast<microsecond>(now_total - start_total_time).count();
}





void measureLogPerformance(const TestType type, long long& test_max_us,std::vector<long long> &measurements, std::string msg)
{
  std::ostringstream oss;
  oss << std::endl << msg << " ";
  { // local scope PRINTF SYNTAX
    long long app_time_us = 0;
    auto start_time = std::chrono::system_clock::now();
    { // local scope - forcing exit
      LogWorker logger(raw_log_name, g_path);
      g2::initializeLogging(&logger);
      app_time_us = iterativeLog(type, measurements);
    } // scope -exit will flush the background logger

    auto now = std::chrono::system_clock::now();
    auto total_exec_time_us = std::chrono::duration_cast<microsecond>(now - start_time).count();
    oss << app_time_us << "," << total_exec_time_us << " us  [OR: ";
    oss << app_time_us/1000 << " , " << total_exec_time_us/1000 << " ms] " << std::endl;
    oss << ": (app) log mean value : " << mean(measurements) << " us" << "\ttotal(app+bg) average time: " <<  total_exec_time_us/k_iterations << " us" << std::endl;

    auto calc_max = std::max_element(measurements.begin(), measurements.end());
    test_max_us = *calc_max;
    oss << ": app max-peak time  : " << test_max_us/1000 << " ms" << std::endl;
    g2::shutDownLogging();
  }
  writeTextToFile(measure_g2log, "\n" + oss.str(), kAppend);
  measurements.clear();
}

} // end anonymous namespace






int main(int argc, char** argv)
{
  std::vector<long long> measurements;
  measurements.reserve(k_iterations);
  std::ostringstream oss;
  oss << "Running #: " << k_loop << " * " << k_iterations << " iterations of log entries" << std::endl;  // worst mean case is about 10us per log entry
  oss << "It can take som time. Please wait: Approximate maximum wait time is:  " << (long long) (k_iterations * 10* 4  / 1000000 ) << " seconds" << std::endl;
  writeTextToFile(measure_g2log, oss.str(), kAppend);
  oss.str(""); // clear the stream

  measureLogPerformance(kTypeSimpleStream, g_simple_max_us, measurements, "#1 SIMPLE LOG STREAM (app/total)");
  measureLogPerformance(kTypeSimpleF, g_simpleF_max_us, measurements, "#2 SIMPLE LOG_F (app/total)");
  measureLogPerformance(kTypeComplexStream, g_complex_max_us, measurements, "#3 Complex LOG STREAM (app/total)");
  measureLogPerformance(kTypeComplexF, g_complexF_max_us, measurements, "#4 Complex LOG_F (app/total)");

  oss << "\nSIMPLE max: " << g_simple_max_us << " us" << "  [" << g_simple_max_us/1000 << " ms]" << std::endl;
  oss << "SIMPLE_F max: " << g_simpleF_max_us << " us" << "  [" << g_simpleF_max_us/1000 << " ms]" << std::endl;
  oss << "COMPLEX max: " << g_complex_max_us << " us" << "  [" << g_complex_max_us/1000 << " ms]" << std::endl;
  oss << "COMPLEX_F max: " << g_complexF_max_us<< " us" << "  [" << g_complexF_max_us/1000 << " ms]" << std::endl;
  writeTextToFile(measure_g2log,oss.str(), kAppend);
  return 0;
}


