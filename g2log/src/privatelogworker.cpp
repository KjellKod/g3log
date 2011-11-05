/* *************************************************
 * Filename: privatelogworker.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include "privatelogworker.h"

#include <sstream>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <ctime>

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



PrivateLogWorker::PrivateLogWorker(const std::string& log_prefix, const std::string& log_directory)
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

PrivateLogWorker::~PrivateLogWorker()
{
  std::ostringstream ss_exit;
  time_t exit_time;
  time(&exit_time);
  bg_.reset(); // flush the log queue
  ss_exit << "\n\t\tg2log file shutdown at: " << ctime(&exit_time);
  out << ss_exit.str() << std::flush;
}

void PrivateLogWorker::backgroundFileWrite(LogEntry message)
{
  using namespace std;
  LogTime t;
  auto timesnapshot = chrono::steady_clock::now();
  out << "\n" << t.year << "/" << setw(2) << t.month << "/" << setw(2) << t.day;
  out << " " << setw(2) << t.hour << ":"<< setw(2) << t.minute <<":"<< setw(2) << t.second;
  out << "." << chrono::duration_cast<microsecond>(timesnapshot - start_time_).count(); //microseconds
  out << "\t" << message;
}

