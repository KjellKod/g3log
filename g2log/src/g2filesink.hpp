#ifndef PRIVATE_G2_FILE_SINK_H_
#define PRIVATE_G2_FILE_SINK_H_



//struct g2FileSink {
//  g2FileSink(const std::string& log_prefix, const std::string& log_directory) {
//    std::string lg = log_prefix;
//    lg.append(":  directory->");
//    lg.append(log_directory);
//    std::cout << lg << std::endl;
//  }
//  
//  template<typename Arg> 
//  void writeToFile(Arg arg) {
//    std::cout << arg << std::endl;
//  }       
//  
//  std::string logFileName() { return ""; }
//};
//        



#include <string>
#include <memory>

#include "g2logmessage.hpp" // TODO refactoring, should include message instead
#include "g2time.hpp"





namespace g2 {

  struct g2FileSink {
    g2FileSink(const std::string& log_prefix, const std::string& log_directory);
    virtual ~g2FileSink();

    void fileWrite(internal::LogEntry message);
    std::string changeLogFile(const std::string& directory);
    std::string fileName();


  private:
   //void backgroundExitFatal(internal::FatalMessage fatal_message);
    void addLogFileHeader();

    std::string _log_file_with_path;
    std::string _log_prefix_backup; // needed in case of future log file changes of directory
    std::unique_ptr<std::ofstream> _outptr;
    g2::steady_time_point _steady_start_time;
    
    
    g2FileSink& operator=(const g2FileSink&); // c++11 feature not yet in vs2010 = delete;
    g2FileSink(const g2FileSink& other); // c++11 feature not yet in vs2010 = delete;

    std::ofstream & filestream() {
      return *(_outptr.get());
    }
  };
} // g2
#endif // pimple
