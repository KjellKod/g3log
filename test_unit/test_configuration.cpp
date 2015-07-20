/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <future>
#include <string>
#include <exception>
#include <functional>
#include <memory>
#include "g3log/time.hpp"
#include "g3log/future.hpp"

TEST(Configuration, LOG)
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

std::future<std::string> sillyFutureReturn()
{
  std::packaged_task<std::string()> task([](){return std::string("Hello Future");}); // wrap the function
  std::future<std::string> result = task.get_future();  // get a future
  std::thread(std::move(task)).detach(); // launch on a thread
  std::cout << "Waiting...";
  result.wait();
  return result; // already wasted
}
TEST(Configuration, FutureSilly)
{
  std::string hello = sillyFutureReturn().get();
  ASSERT_STREQ(hello.c_str(), "Hello Future");
}

struct MsgType
{
  std::string msg_;
  MsgType(std::string m): msg_(m){};
  std::string msg(){return msg_;}
};


TEST(TestOf_CopyableCall, Expecting_SmoothSailing)
{
  using namespace kjellkod;
  const std::string str("Hello from struct");
  MsgType type(str);
  std::unique_ptr<Active> bgWorker(Active::createActive());
  std::future<std::string> fstring =
    g3::spawn_task(std::bind(&MsgType::msg, type), bgWorker.get());
  ASSERT_STREQ(str.c_str(), fstring.get().c_str());
}



TEST(TestOf_CopyableLambdaCall, Expecting_AllFine)
{
  using namespace kjellkod;
  std::unique_ptr<Active> bgWorker(Active::createActive());

  // lambda task
  const std::string str_standalone("Hello from standalone");
  auto msg_lambda=[=](){return (str_standalone+str_standalone);};
  std::string expected(str_standalone+str_standalone);

  auto fstring_standalone = g3::spawn_task(msg_lambda, bgWorker.get());
  ASSERT_STREQ(expected.c_str(), fstring_standalone.get().c_str());
}




template<typename F>
std::future<typename std::result_of<F()>::type> ObsoleteSpawnTask(F f)
{
  typedef typename std::result_of<F()>::type result_type;
  typedef std::packaged_task<result_type()> task_type;

  task_type task(std::move(f));
  std::future<result_type> result = task.get_future();

  std::vector<std::function<void()>> vec;
  vec.push_back(g3::MoveOnCopy<task_type>(std::move(task)));
  std::thread(std::move(vec.back())).detach();
  result.wait();
  return std::move(result);
}

TEST(TestOf_ObsoleteSpawnTaskWithStringReturn, Expecting_FutureString)
{
  std::string str("Hello");
  std::string expected(str+str);
  auto msg_lambda=[=](){return (str+str);};
  auto future_string = ObsoleteSpawnTask(msg_lambda);

  ASSERT_STREQ(expected.c_str(), future_string.get().c_str());
}
// gcc thread example below
// tests code below copied from mail-list conversion between
// Lars Gullik Bjønnes and Jonathan Wakely
// http://gcc.gnu.org/ml/gcc-help/2011-11/msg00052.html

// --------------------------------------------------------------
namespace WORKING
{
  using namespace g3;

#include <gtest/gtest.h>

#include <iostream>
#include <future>
#include <thread>
#include <vector>

  std::vector<std::function<void()>> vec;

  template<typename F>
  std::future<typename std::result_of<F()>::type> spawn_task(F f)
  {
    typedef typename std::result_of<F()>::type result_type;
    typedef std::packaged_task<result_type()> task_type;

    task_type task(std::move(f));
    std::future<result_type> res = task.get_future();

    vec.push_back(
      MoveOnCopy<task_type>(
      std::move(task)));

    std::thread([]()
    {
      auto task = std::move(vec.back());
      vec.pop_back();
      task();
    }
    ).detach();

    return std::move(res);
  }



  double get_res()
  {
    return 42.2;
  }

  std::string msg3(){return "msg3";}
} // WORKING

TEST(Yalla, Testar)
{
  using namespace WORKING;
  auto f = spawn_task(get_res);
  std::cout << "Res = " << f.get() << std::endl;

  auto f2 = spawn_task(msg3);
  std::cout << "Res2 = " << f2.get() << std::endl;


  ASSERT_TRUE(true);
}





