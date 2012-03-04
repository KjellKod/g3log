/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/
#ifndef PERFORMANCE_G2_TEST_H_
#define PERFORMANCE_G2_TEST_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cassert>

#if defined(G2LOG_PERFORMANCE)
#include "g2log.h"
#include "g2logworker.h"
#elif defined(GOOGLE_GLOG_PERFORMANCE)
#include <glog/logging.h>
#else
#error G2LOG_PERFORMANCE or GOOGLE_GLOG_PERFORMANCE was not defined
#endif


// typedef std::chrono::steady_clock::time_point time_point;
// typedef std::chrono::duration<long,std::ratio<1, 1000> > millisecond;
// typedef std::chrono::duration<long long,std::ratio<1, 1000000> > microsecond;
// typedef std::chrono::duration<long long, std::ratio<1, 1000000> > microsecond;
namespace g2_test
{
enum WriteMode
{
  kAppend = 0,
  kTruncate = 1
};

const size_t g_loop = 1;
const size_t g_iterations = 1000000;
const char* charptrmsg = "\tmessage by char*";
const std::string strmsg = "\tmessage by string";
float pi_f = 3.1415926535897932384626433832795f;


bool writeTextToFile(const std::string& filename, const std::string& msg, const WriteMode write_mode, bool push_out = true)
{
  if(push_out)
  {
    std::cout << msg << std::flush;
  }

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




void measurePeakDuringLogWrites(const std::string& title, std::vector<long long>& result);
inline void measurePeakDuringLogWrites(const std::string& title, std::vector<long long>& result)
{
#if defined(G2LOG_PERFORMANCE)
  std::cout << "G2LOG (" << title << ") WORST_PEAK PERFORMANCE TEST" << std::endl;
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  std::cout << "GOOGLE_GLOG (" << title << ") WORST_PEAK PERFORMANCE TEST" << std::endl;
#else
  std::cout << "ERROR no performance type chosen" << std::endl;
  assert(false);
#endif
  for(size_t count = 0; count < g_iterations; ++count)
  {
    auto start_time = std::chrono::steady_clock::now();
    LOG(INFO) << title << " iteration #" << count << " " << charptrmsg << strmsg << " and a float: " << std::setprecision(6) << pi_f;
    auto stop_time = std::chrono::steady_clock::now();
    auto time_us = std::chrono::duration_cast<microsecond>(stop_time - start_time).count();
    result.push_back(time_us);
  }
}


void doLogWrites(const std::string& title);
inline void doLogWrites(const std::string& title)
{
#if defined(G2LOG_PERFORMANCE)
  std::cout << "G2LOG (" << title << ") PERFORMANCE TEST" << std::endl;
#elif defined(GOOGLE_GLOG_PERFORMANCE)
  std::cout << "GOOGLE_GLOG (" << title << ") PERFORMANCE TEST" << std::endl;
#else
  std::cout << "ERROR no performance type chosen" << std::endl;
  assert(false);
#endif
  for(size_t count = 0; count < g_iterations; ++count)
  {
    LOG(INFO) << title << " iteration #" << count << " " << charptrmsg << strmsg << " and a float: " << std::setprecision(6) << pi_f;
  }
}


} // end namespace


#endif // fPERFORMANCE_G2_TEST_H_
