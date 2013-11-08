// -*- C++ -*-
/* 
 * File:   g2logmessageimpl.hpp
 * Author: kjell
 *
 * Created on October 27, 2013, 9:14 PM
 */

#pragma once


#include <string>
#include "g2loglevels.hpp"
#include <ctime>
#include <sstream>

namespace g2 {
struct LogMessageImpl {
   
  // LOG message constructor
   LogMessageImpl(const std::string &file, const int line,
           const std::string& function, const LEVELS& level);

   // Fatal OS-Signal constructor 
   explicit LogMessageImpl(const std::string& fatalOsSignalCrashMessage);

   LogMessageImpl(const LogMessageImpl& copy) = default;
   ~LogMessageImpl() = default;

   const std::time_t _timestamp;
   const long _microseconds;
   const std::string _file;
   const int _line;
   const std::string _function;
   const LEVELS _level;

   std::string _expression; // only with content for CHECK(...) calls
   std::ostringstream _stream;
};
}
