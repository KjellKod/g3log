/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2logmessage.hpp  Part of Framework for Logging and Design By Contract
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#ifndef G2_LOG_MESSAGE_HPP
#define G2_LOG_MESSAGE_HPP


#include <string>
#include <sstream>
#include <iostream>
#include <cstdarg>
#include <memory>

#include "g2log.hpp"
#include "g2loglevels.hpp"

struct LogEntryMessageImpl;
// Notice: Currently the LogEntryMessage does not use polymorphism to
// implement the "expression" part even though it is only used for CHECK(...)
// contracts and not by normal messages
class LogEntryMessage
{
  std::unique_ptr<LogEntryMessageImpl> pimpl_;
  std::string file();
  std::string line();
  std::string function();
  std::string level();
  std::string timestamp();
  std::string microseconds();
  std::string message();
  std::string expression();

  std::ostringstream&  saveMessageByStream();
#ifndef __GNUC__
#define  __attribute__(x) // Disable 'attributes' if compiler does not support 'em
#endif
  // Coder note: Since it's C++ and not C EVERY CLASS FUNCTION always get a first
  // compiler given argument 'this'. This must be supplied as well, hence '2,3'
  // ref: http://www.codemaestro.com/reviews/18 -- ref KjellKod
  void saveMessage(const char *printf_like_message, ...)
  __attribute__((format(printf,2,3) ));

public:
  // replace for factory function instead
  // createContractMessage
  // createMessage
  LogEntryMessage(const std::string &file, const int line, const std::string& function, const LEVELS& level);
  LogEntryMessage(const std::string &file, const int line, const std::string& function, const LEVELS& level, const std::string boolean_expression);
};




namespace g2 {  namespace internal {

    // temporary message construct to capture log input and push to g2logworker
    class LogMessage
    {
    public:
      LogMessage(const std::string &file, const int line, const std::string& function, const LEVELS& level);
      virtual ~LogMessage(); // flush the message
      std::ostringstream& messageStream(){return stream_;}


      // The __attribute__ generates compiler warnings if illegal "printf" format
      // IMPORTANT: You muse enable the compiler flag '-Wall' for this to work!
      // ref: http://www.unixwiz.net/techtips/gnu-c-attributes.html
#ifndef __GNUC__
#define  __attribute__(x) // Disable 'attributes' if compiler does not support 'em
#endif
      // Coder note: Since it's C++ and not C EVERY CLASS FUNCTION always get a first
      // compiler given argument 'this'. This must be supplied as well, hence '2,3'
      // ref: http://www.codemaestro.com/reviews/18 -- ref KjellKod
      void messageSave(const char *printf_like_message, ...)
      __attribute__((format(printf,2,3) ));


    protected:
      const std::string file_;
      const int line_;
      const std::string function_;
      const LEVELS& level_;
      std::ostringstream stream_;
      std::string log_entry_;
    };


    // 'Design-by-Contract' temporary messsage construction
    class LogContractMessage : public LogMessage
    {
    public:
      LogContractMessage(const std::string &file, const int line,
                         const std::string &function, const std::string &boolean_expression);
      virtual ~LogContractMessage(); // at destruction will flush the message

    protected:
      const std::string expression_;
    };
  } // internal
             } // g2


#endif //G2_LOG_MESSAGE_HPP
