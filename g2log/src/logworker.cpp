/* *************************************************
 * Filename:logworker.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include "logworker.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <ctime>

 #if defined(YALLA)
#include <stdexcept> // exceptions
#endif

#include "active.h"
#include "g2log.h"
#include "crashhandler.h"

using namespace g2::internal;
namespace
{
struct LogTime
{
  LogTime()
  {
    time_t current_time = time(nullptr);
    ctime(&current_time); // fill with time right now
    struct tm* ptm = localtime(&current_time); // fill time struct with data
    year = ptm->tm_year + 1900;
    month = (ptm->tm_mon) +1;
    day = ptm->tm_mday;
    hour = ptm->tm_hour;
    minute = ptm->tm_min;
    second = ptm->tm_sec;
  }
  int year;   // Year	- 1900
  int month;  // [1-12]
  int day;    // [1-31]
  int hour;   // [0-23]
  int minute; // [0-59]
  int second; // [0-60], 1 leap second
};
}  // end anonymous namespace




/** The actual background worker, while LogWorker gives the
  * asynchronous API to put job in the background the LogWorkerImpl
  * does the actual background thread work */
struct LogWorkerImpl
{
  LogWorkerImpl(const std::string& log_prefix, const std::string& log_directory);
  ~LogWorkerImpl();

  void backgroundFileWrite(g2::internal::LogEntry message);
  void backgroundExitFatal(g2::internal::FatalMessage fatal_message);

  std::string log_file_with_path_;
  std::unique_ptr<kjellkod::Active> bg_;
  std::ofstream out;
  g2::internal::time_point start_time_;

private:
  LogWorkerImpl& operator=(const LogWorkerImpl&); // c++11 feature not yet in vs2010 = delete;
  LogWorkerImpl(const LogWorkerImpl& other); // c++11 feature not yet in vs2010 = delete;
};



//
// Private API implementation : LogWorkerImpl
LogWorkerImpl::LogWorkerImpl(const std::string& log_prefix, const std::string& log_directory)
  : log_file_with_path_(log_directory)
  , bg_(kjellkod::Active::createActive())
  , start_time_(std::chrono::steady_clock::now())
{
  using namespace std;
  LogTime t;
  ostringstream oss_name;
  oss_name.fill('0');
  oss_name << log_prefix << ".g2log.";
  oss_name << t.year << setw(2) << t.month << setw(2) << t.day;
  oss_name << "-" << setw(2) << t.hour << setw(2) << t.minute << setw(2) << t.second;
  oss_name << ".log";
  log_file_with_path_ += oss_name.str();

  // open the log file
  std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
  mode |= std::ios_base::trunc;
  out.open(log_file_with_path_, mode);
  if(!out.is_open())
  {
    std::ostringstream ss_error;
    ss_error << "Fatal error could not open log file:[" << log_file_with_path_ << "]";
    ss_error << "\n\t\t std::ios_base state = " << out.rdstate();
    std::cerr << ss_error.str().c_str() << std::endl << std::flush;
    assert(false && "cannot open log file at startup");
  }
  std::ostringstream ss_entry;
  time_t creation_time;
  time(&creation_time);
  ss_entry << "\t\tg2log created log file at: " << ctime(&creation_time);
  ss_entry << "\t\tLOG format: [YYYY/MM/DD hh:mm:ss.uuu* LEVEL FILE:LINE] message\n\n";
  out << ss_entry.str() << std::flush;
  out.fill('0');
}

LogWorkerImpl::~LogWorkerImpl()
{
  std::ostringstream ss_exit;
  time_t exit_time;
  time(&exit_time);
  bg_.reset(); // flush the log queue
  ss_exit << "\n\t\tg2log file shutdown at: " << ctime(&exit_time);
  out << ss_exit.str() << std::flush;
}

void LogWorkerImpl::backgroundFileWrite(LogEntry message)
{
  using namespace std;
  LogTime t;
  auto timesnapshot = chrono::steady_clock::now();
  out << "\n" << t.year << "/" << setw(2) << t.month << "/" << setw(2) << t.day;
  out << " " << setw(2) << t.hour << ":"<< setw(2) << t.minute <<":"<< setw(2) << t.second;
  out << "." << chrono::duration_cast<microsecond>(timesnapshot - start_time_).count(); //microseconds
  out << "\t" << message << std::flush;
}

void LogWorkerImpl::backgroundExitFatal(FatalMessage fatal_message)
{
  backgroundFileWrite(fatal_message.message_);

 #if defined(YALLA)
  // If running unit test - we simplify matters by not sending the signal, but
  // by just throwing an exception
  throw std::runtime_error(fatal_message.message_);
  return;
#endif

  out.close();
  exitWithDefaultSignalHandler(fatal_message.signal_id_);
  perror("g2log exited after receiving FATAL trigger. Flush message status: "); // should never reach this point
}



// BELOW LogWorker
// Public API implementation
 LogWorker::LogWorker(const std::string& log_prefix, const std::string& log_directory)
   :  pimpl_(new LogWorkerImpl(log_prefix, log_directory))
   , log_file_with_path_(pimpl_->log_file_with_path_)
 {
 }

 LogWorker::~LogWorker()
 {
   pimpl_.reset();
   std::cout << "\nExiting, log location: " << log_file_with_path_ << std::endl << std::flush;
 }

 void LogWorker::save(g2::internal::LogEntry msg)
 {
   pimpl_->bg_->send(std::tr1::bind(&LogWorkerImpl::backgroundFileWrite, pimpl_.get(), msg));
 }

 void LogWorker::fatal(g2::internal::FatalMessage fatal_message)
 {
   pimpl_->bg_->send(std::tr1::bind(&LogWorkerImpl::backgroundExitFatal, pimpl_.get(), fatal_message));
 }

 std::string LogWorker::logFileName() const
 {
   return log_file_with_path_;
 }

