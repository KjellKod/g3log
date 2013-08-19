#include "g2filesink.hpp"
#include "g2filesinkhelper.ipp"

#include <cassert>

namespace g2 {
  using namespace internal;
  
  g2FileSink::g2FileSink(const std::string& log_prefix, const std::string& log_directory)
  : _log_file_with_path(log_directory)
  , _log_prefix_backup(log_prefix)
  , _outptr(new std::ofstream)
  , _steady_start_time(std::chrono::steady_clock::now()) // TODO: ha en timer function steadyTimer som har koll på start
  {
    _log_prefix_backup = prefixSanityFix(log_prefix);
    if (!isValidFilename(_log_prefix_backup)) {
      std::cerr << "g2log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
      abort();
    }

    std::string file_name = createLogFileName(_log_prefix_backup);
    _log_file_with_path = pathSanityFix(_log_file_with_path, file_name);
    _outptr = createLogFile(_log_file_with_path);

    if (!_outptr) {
      std::cerr << "Cannot write log file to location, attempting current directory" << std::endl;
      _log_file_with_path = "./" + file_name;
      _outptr = createLogFile(_log_file_with_path);
    }
    assert(_outptr  && "cannot open log file at startup");
    addLogFileHeader();
  }
  
  

  g2FileSink::~g2FileSink() {
    std::string exit_msg {"\n\t\tg2log g2FileSink shutdown at: "};
    exit_msg.append(localtime_formatted(systemtime_now(), internal::time_formatted));
    filestream() << exit_msg << std::flush;
    
    exit_msg.append({"\nLog file at: ["}).append(_log_file_with_path).append({"]\n\n"});
    std::cerr << exit_msg << std::flush;
  }

  void g2FileSink::fileWrite(internal::LogEntry message) {
    std::ofstream & out(filestream());
    auto system_time = systemtime_now();
    auto steady_time = std::chrono::steady_clock::now();
    out << "\n" << localtime_formatted(system_time, date_formatted);
    out << " " << localtime_formatted(system_time, time_formatted); // TODO: time kommer från LogEntry
    out << "." << std::chrono::duration_cast<std::chrono::microseconds>(steady_time - _steady_start_time).count();
    out << "\t" << message << std::flush;
  }

//  void g2FileSink::backgroundExitFatal(internal::FatalMessage fatal_message) {
//
//    backgroundFileWrite(fatal_message.message_);
//    backgroundFileWrite("Log flushed successfully to disk \nExiting");
//    std::cerr << "g2log exiting after receiving fatal event" << std::endl;
//    std::cerr << "Log file at: [" << log_file_with_path_ << "]\n" << std::endl << std::flush;
//    filestream().close();
//    exitWithDefaultSignalHandler(fatal_message.signal_id_);
//    perror("g2log exited after receiving FATAL trigger. Flush message status: "); // should never reach this point
//  }

  std::string g2FileSink::changeLogFile(const std::string& directory) {
    std::string file_name = createLogFileName(_log_prefix_backup);
    std::string prospect_log = directory + file_name;
    std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
    if (nullptr == log_stream) {
      fileWrite("Unable to change log file. Illegal filename or busy? Unsuccessful log name was:" + prospect_log);
      return ""; // no success
    }

    addLogFileHeader();
    std::ostringstream ss_change;
    ss_change << "\n\tChanging log file from : " << _log_file_with_path;
    ss_change << "\n\tto new location: " << prospect_log << "\n";
    fileWrite(ss_change.str().c_str());
    ss_change.str("");

    std::string old_log = _log_file_with_path;
    _log_file_with_path = prospect_log;
    _outptr = std::move(log_stream);
    ss_change << "\n\tNew log file. The previous log file was at: ";
    ss_change << old_log;
    fileWrite(ss_change.str());
    return _log_file_with_path;
  }

  std::string g2FileSink::fileName() {
    return _log_file_with_path;
  }

  void g2FileSink::addLogFileHeader() {
      filestream() << header();
  }

} // g2