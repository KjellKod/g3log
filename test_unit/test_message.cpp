/** ==========================================================================
* 2016 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <gtest/gtest.h>
#include <g3log/g3log.hpp>
#include <g3log/time.hpp>
#include <iostream>

 TEST(Message, CppSupport)
{ // ref: http://www.cplusplus.com/reference/clibrary/ctime/strftime/
  // ref: http://en.cppreference.com/w/cpp/io/manip/put_time
  //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
  // --- WARNING: The try/catch setup does NOT work,. but for fun and for fake-clarity I leave it
  // ---  For formatting options to std::put_time that are NOT YET implemented on Windows fatal errors/assert will occurr
  // ---  the last example is such an example.
  try
  {
    std::cout << g3::localtime_formatted(g3::systemtime_now(), "%a %b %d %H:%M:%S %Y")  << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << g3::localtime_formatted(g3::systemtime_now(), "%%Y/%%m/%%d %%H:%%M:%%S = %Y/%m/%d %H:%M:%S")  << std::endl;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    std::cerr << "Formatting options skipped due to VS2012, C++11 non-conformance for" << std::endl;
    std::cerr << " some formatting options. The skipped code was:\n\t\t %EX %Ec, \n(see http://en.cppreference.com/w/cpp/io/manip/put_time for details)"  << std::endl;
#else
    std::cout << "C++11 new formatting options:\n" << g3::localtime_formatted(g3::systemtime_now(), "%%EX: %EX\n%%z: %z\n%%Ec: %Ec")  << std::endl;
#endif
  }
// This does not work. Other kinds of fatal exits (on Windows) seems to be used instead of exceptions
// Maybe a signal handler catch would be better? --- TODO: Make it better, both failing and correct
catch(...)
{
  ADD_FAILURE() << "On this platform the library does not support given (C++11?) specifiers";
  return;
}
ASSERT_TRUE(true); // no exception. all good
}

// testing is missing for
// time.cpp 
//      fractional_to_string
//      size_t fractional_cutoff
//      localtime_formatted
//
// -- change
// time.cpp naming
// confusing comment + assert
// fractional_to_string, take the whole timespec value?
//
// -- to find out
// -lrt?