/* 
 * File:   test_helper__restore_logger.h
 * Author: kjell
 *
 * Created on July 13, 2013, 4:46 PM
 */

#ifndef TEST_HELPER__RESTORE_LOGGER_H
#define	TEST_HELPER__RESTORE_LOGGER_H

#include <memory>
#include <string>
#include <iostream>
class g2LogWorker;


// After initializing ScopedCout all std::couts is redirected to the buffer
// Example: 
//   stringstream buffer;   
//   ScopedCout guard(&buffer);
//   cout << "Hello World";
//   ASSERT_STREQ(buffer.str().c_str(), "Hello World"); 
class ScopedCout
{
   std::streambuf*  _old_cout;
   public:
    explicit ScopedCout(std::stringstream*  buffer);
    ~ScopedCout();
};



// RAII temporarily replace of logger
// and restoration of original logger at scope end
struct RestoreLogger
{
	explicit RestoreLogger(std::string directory);
	~RestoreLogger();
	void reset();

	std::unique_ptr<g2LogWorker> logger_;
	std::string logFile(){return log_file_;}
private:
	std::string log_file_;

};

#endif	/* TEST_HELPER__RESTORE_LOGGER_H */

