/* *************************************************
 * Filename: main.cpp
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copy-writed
 * ********************************************* */

#include "logworker.h"
#include "g2log.h"
#include "logworker.h"
#include <iomanip>

int main(int argc, char** argv)
{
  double pi_d = 3.1415926535897932384626433832795;
  float pi_f = 3.1415926535897932384626433832795f;


  LogWorker logger(argv[0], "/tmp/");
  g2::initializeLogging(&logger);

  std::cout << "****** A NUMBER of 'runtime exceptions' will be printed on this screen" << std::endl;
  std::cout << "******    that's all good and part of the 'example'. " << std::endl;
  std::cout << "******    please see g2log/src/main.cpp and he finished log file to " << std::endl;
  std::cout << "******    follow what is done in this example\n\n" << std::endl;

  LOGF(INFO, "Hi log %d", 123);
  LOG(INFO) << "Test SLOG INFO";
  LOG(DEBUG) << "Test SLOG DEBUG";
  LOG(INFO) << "one: " << 1;
  LOG(INFO) << "two: " << 2;
  LOG(INFO) << "one and two: " << 1 << " and " << 2;
  LOG(DEBUG) << "float 2.14: " << 1000/2.14f;
  LOG(DEBUG) << "pi double: " << pi_d;
  LOG(DEBUG) << "pi float: " << pi_f;
  LOG(DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
  LOGF(INFO, "pi float printf:%f", pi_f);

  //
  // START: LOG Entris that were in the article
  //
  //LOG(UNKNOWN_LEVEL) << "This log attempt will cause a compiler error";
  LOG(INFO) << "Simple to use with streaming syntax, easy as abc or " << 123;
  LOGF(WARNING, "Printf-style syntax is also %s", "available");
  // ....
  try
  {
    LOGF(FATAL, "FATAL has a special meaning. This %s will throw an exception", "message");
  }
  catch(...)
  {
    std::cout << "\n **** All good expected the 'FATAL has a special meaning' runtime exception\n\n\n" << std::endl;
  }
  LOG_IF(INFO, (1 < 2)) << "If true this text will be logged";
  LOGF_IF(INFO, (1<2), "if %d<%d : then this text will be logged", 1,2);
  LOG_IF(FATAL, (2>3)) << "This message should NOT throw";
  LOGF(DEBUG, "This API is popular with some %s", "programmers");
  LOGF_IF(DEBUG, (1<2), "If true, then this %s will be logged", "message");
  {
    const std::string logging = "logging";
    // OK --- this WILL get a compiler warning
    LOGF(DEBUG, "Printf-type %s is the number 1 for many %s", logging.c_str());
  }
  CHECK(1 != 2); // true: won't throw
  try
  {
    CHECK(1 > 2) << "CHECK(false) will put this message to the throw exception message and our log";
  }
  catch(...)
  {
    std::cout << "\n **** All good expected the 'CHECK(false) will put ...' runtime exception\n\n\n" << std::endl;
  }
  //
  // END: LOG Entris that were in the article
  //
  try
  {
    const std::string arg = "CHECK_F";
    CHECK_F(1 > 2, "This is a test to see if %s works", arg.c_str());
  }
  catch(...)
  {
    std::cout << "\n **** All good expected the 'CHECK(1>2) This is a test ...' runtime exception\n\n\n" << std::endl;
  }


  try
  {
    std::cout << "\n\n***** Be ready the example will show a 'runtime exception' " << std::endl;
    CHECK(1<2) << "SHOULD NOT SEE THIS MESSAGE";
    CHECK(1>2) << "Test to see if contract works: onetwothree: " << 123 << ". This should be inside an exception";
  }
  catch(std::exception& exc)
  {
    std::cout << "\n*****  All good, the 'exception' was part of the example\n\n\n" << std::endl;
    return 0;
  }
  std::cerr << "Unexpected ending, expected an exception" << std::endl << std::flush;
  return 1;
}
