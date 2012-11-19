/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2LogWorker.cpp  Framework for Logging and Design By Contract
* Created: 2011 by Kjell Hedström
*
* PUBLIC DOMAIN and Not under copywrite protection. First published at KjellKod.cc
* ********************************************* */

#include "g2logworker.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <string>
#include <chrono>
#include <future>
#include <functional>


#include "active.h"
#include "g2log.h"
#include "crashhandler.h"
#include "g2time.h"
#include "g2future.h"

using namespace g2;
using namespace g2::internal;

namespace
{
static const std::string date_formatted =  "%Y/%m/%d";
static const std::string time_formatted = "%H:%M:%S";
static const std::string file_name_time_formatted =  "%Y%m%d-%H%M%S";

// check for filename validity -  filename should not be part of PATH
bool isValidFilename(const std::string prefix_filename)
{

  std::string illegal_characters("/,|<>:#$%{}()[]\'\"^!?+* ");
  size_t pos = prefix_filename.find_first_of(illegal_characters,0);
  if(pos != std::string::npos)
  {
    std::cerr << "Illegal character [" << prefix_filename.at(pos) << "] in logname prefix: " << "[" << prefix_filename << "]" << std::endl;
    return false;
  }
  else if (prefix_filename.empty())
  {
    std::cerr << "Empty filename prefix is not allowed" << std::endl;
    return false;
  }

  return true;
}


// Clean up the path if put in by mistake in the prefix
std::string prefixSanityFix(std::string prefix)
{
  prefix.erase(std::remove_if(prefix.begin(), prefix.end(), ::isspace), prefix.end());
  prefix.erase(std::remove( prefix.begin(), prefix.end(), '/'), prefix.end()); // '/'
  prefix.erase(std::remove( prefix.begin(), prefix.end(), '\\'), prefix.end()); // '\\'
  prefix.erase(std::remove( prefix.begin(), prefix.end(), '.'), prefix.end());  // '.'
  if(!isValidFilename(prefix))
  {
    return "";
  }
  return prefix;
}


std::string createLogFileName(const std::string& verified_prefix)
{
  std::stringstream oss_name;
  oss_name.fill('0');
  oss_name << verified_prefix << ".g2log.";
  oss_name << g2::localtime_formatted(g2::systemtime_now(), file_name_time_formatted);
  oss_name << ".log";
  return oss_name.str();
}


bool openLogFile(const std::string& complete_file_with_path, std::ofstream& outstream)
{
  std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
  mode |= std::ios_base::trunc;
  outstream.open(complete_file_with_path, mode);
  if(!outstream.is_open())
  {
    std::ostringstream ss_error;
    ss_error << "FILE ERROR:  could not open log file:[" << complete_file_with_path << "]";
    ss_error << "\n\t\t std::ios_base state = " << outstream.rdstate();
    std::cerr << ss_error.str().c_str() << std::endl << std::flush;
    outstream.close();
    return false;
  }
  std::ostringstream ss_entry;
  //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
  ss_entry << "\t\tg2log created log file at: "<< g2::localtime_formatted(g2::systemtime_now(), "%a %b %d %H:%M:%S %Y") << "\n";
  ss_entry << "\t\tLOG format: [YYYY/MM/DD hh:mm:ss.uuu* LEVEL FILE:LINE] message\n\n"; // TODO: if(header)
  outstream << ss_entry.str() << std::flush;
  outstream.fill('0');
  return true;
}


std::unique_ptr<std::ofstream> createLogFile(const std::string& file_with_full_path)
{
  std::unique_ptr<std::ofstream> out(new std::ofstream);
  std::ofstream& stream(*(out.get()));
  bool success_with_open_file = openLogFile(file_with_full_path, stream);
  if(false == success_with_open_file)
  {
    out.release(); // nullptr contained ptr<file> signals error in creating the log file
  }
  return out;
}
}  // end anonymous namespace


/** The Real McCoy Background worker, while g2LogWorker gives the
* asynchronous API to put job in the background the g2LogWorkerImpl
* does the actual background thread work */
struct g2LogWorkerImpl
{
  g2LogWorkerImpl(const std::string& log_prefix, const std::string& log_directory);
  ~g2LogWorkerImpl();

  void backgroundFileWrite(g2::internal::LogEntry message);
  void backgroundExitFatal(g2::internal::FatalMessage fatal_message);
  std::string  backgroundChangeLogFile(const std::string& directory);
  std::string  backgroundFileName();

  std::string log_file_with_path_;
  std::string log_prefix_backup_; // needed in case of future log file changes of directory
  std::unique_ptr<kjellkod::Active> bg_;
  std::unique_ptr<std::ofstream> outptr_;
  steady_time_point steady_start_time_;

private:
  g2LogWorkerImpl& operator=(const g2LogWorkerImpl&); // c++11 feature not yet in vs2010 = delete;
  g2LogWorkerImpl(const g2LogWorkerImpl& other); // c++11 feature not yet in vs2010 = delete;
  std::ofstream& filestream(){return *(outptr_.get());}
};



//
// Private API implementation : g2LogWorkerImpl
g2LogWorkerImpl::g2LogWorkerImpl(const std::string& log_prefix, const std::string& log_directory)
  : log_file_with_path_(log_directory)
  , log_prefix_backup_(log_prefix)
  , bg_(kjellkod::Active::createActive())
  , outptr_(new std::ofstream)
  , steady_start_time_(std::chrono::steady_clock::now()) // TODO: ha en timer function steadyTimer som har koll på start
{
  log_prefix_backup_ = prefixSanityFix(log_prefix);
  if(!isValidFilename(log_prefix_backup_))
  {
    // illegal prefix, refuse to start
    std::cerr << "g2log: forced abort due to illegal log prefix [" << log_prefix <<"]" << std::endl << std::flush;
    abort();
  }

  std::string file_name = createLogFileName(log_prefix_backup_);
  log_file_with_path_ = log_directory + file_name;
  outptr_ = createLogFile(log_file_with_path_);
  assert((nullptr != outptr_) && "cannot open log file at startup");
}


g2LogWorkerImpl::~g2LogWorkerImpl()
{
  std::ostringstream ss_exit;
  bg_.reset(); // flush the log queue
  ss_exit << "\n\t\tg2log file shutdown at: " << g2::localtime_formatted(g2::systemtime_now(), time_formatted);
  filestream() << ss_exit.str() << std::flush;
}


void g2LogWorkerImpl::backgroundFileWrite(LogEntry message)
{
  using namespace std;
  std::ofstream& out(filestream());
  auto system_time = g2::systemtime_now();
  auto steady_time = std::chrono::steady_clock::now();
  out << "\n" << g2::localtime_formatted(system_time, date_formatted);
  out << " " << g2::localtime_formatted(system_time, time_formatted); // TODO: time kommer från LogEntry
  out << "." << chrono::duration_cast<std::chrono::microseconds>(steady_time - steady_start_time_).count(); //microseconds TODO: ta in min g2clocka här StopWatch
  out << "\t" << message << std::flush;
}


void g2LogWorkerImpl::backgroundExitFatal(FatalMessage fatal_message)
{
  backgroundFileWrite(fatal_message.message_);
  backgroundFileWrite("Log flushed successfully to disk \nExiting");
  std::cerr << "g2log exiting after receiving fatal event" << std::endl;
  std::cerr << "Log file at: [" << log_file_with_path_ << "]\n" << std::endl << std::flush;
  filestream().close();
  exitWithDefaultSignalHandler(fatal_message.signal_id_);
  perror("g2log exited after receiving FATAL trigger. Flush message status: "); // should never reach this point
}


std::string g2LogWorkerImpl::backgroundChangeLogFile(const std::string& directory)
{
  std::string file_name = createLogFileName(log_prefix_backup_);
  std::string prospect_log = directory + file_name;
  std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
  if(nullptr == log_stream)
  {
    backgroundFileWrite("Unable to change log file. Illegal filename or busy? Unsuccessful log name was:" + prospect_log);
    return ""; // no success
  }

  std::ostringstream ss_change;
  ss_change << "\n\tChanging log file from : " << log_file_with_path_;
  ss_change << "\n\tto new location: " << prospect_log << "\n";
  backgroundFileWrite(ss_change.str().c_str());
  ss_change.str("");

  // setting the new log as active
  std::string old_log = log_file_with_path_;
  log_file_with_path_ = prospect_log;
  outptr_ = std::move(log_stream);
  ss_change << "\n\tNew log file. The previous log file was at: ";
  ss_change << old_log;
  backgroundFileWrite(ss_change.str());
  return log_file_with_path_;
}

std::string  g2LogWorkerImpl::backgroundFileName()
{
  return log_file_with_path_;
}




//
// *****   BELOW g2LogWorker    *****
// Public API implementation
//
g2LogWorker::g2LogWorker(const std::string& log_prefix, const std::string& log_directory)
  :  pimpl_(new g2LogWorkerImpl(log_prefix, log_directory))
  , log_file_with_path_(pimpl_->log_file_with_path_)
{
  assert((pimpl_ != nullptr) && "shouild never happen");
}

g2LogWorker::~g2LogWorker()
{
  pimpl_.reset();
  std::cerr << "\nExiting, log location: " << log_file_with_path_ << std::endl << std::flush;
}

void g2LogWorker::save(g2::internal::LogEntry msg)
{
  pimpl_->bg_->send(std::bind(&g2LogWorkerImpl::backgroundFileWrite, pimpl_.get(), msg));
}

void g2LogWorker::fatal(g2::internal::FatalMessage fatal_message)
{
  pimpl_->bg_->send(std::bind(&g2LogWorkerImpl::backgroundExitFatal, pimpl_.get(), fatal_message));
}


std::future<std::string> g2LogWorker::changeLogFile(const std::string& log_directory)
{
  kjellkod::Active* bgWorker = pimpl_->bg_.get();
  //auto future_result = g2::spawn_task(std::bind(&g2LogWorkerImpl::backgroundChangeLogFile, pimpl_.get(), log_directory), bgWorker);
   auto bg_call =     [this, log_directory]() {return pimpl_->backgroundChangeLogFile(log_directory);};
   auto future_result = g2::spawn_task(bg_call, bgWorker);
  return std::move(future_result);
}

std::future<std::string> g2LogWorker::logFileName()
{
  kjellkod::Active* bgWorker = pimpl_->bg_.get();
  auto bg_call=[&](){return pimpl_->backgroundFileName();};
  auto future_result = g2::spawn_task(bg_call ,bgWorker);
  return std::move(future_result);
}
