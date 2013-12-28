#ifndef PRIVATE_G2_FILE_SINK_H_
#define PRIVATE_G2_FILE_SINK_H_

/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*/

#include <string>
#include <memory>

#include "g2logmessage.hpp" // TODO refactoring, should include message instead

namespace g2 {

class FileSink {
public:
   FileSink(const std::string& log_prefix, const std::string& log_directory);
   virtual ~FileSink();

   void fileWrite(LogMessageMover message);
   std::string changeLogFile(const std::string& directory);
   std::string fileName();


private:
   std::string _log_file_with_path;
   std::string _log_prefix_backup; // needed in case of future log file changes of directory
   std::unique_ptr<std::ofstream> _outptr;

   void addLogFileHeader();
   std::ofstream & filestream() {return *(_outptr.get()); }


   FileSink& operator=(const FileSink&) = delete;
   FileSink(const FileSink& other) = delete;

};
} // g2
#endif // pimple
