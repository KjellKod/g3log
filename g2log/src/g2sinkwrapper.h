/* 
 * File:   g2sinkwrapper.ipp
 * Author: kjell
 *
 * Created on July 21, 2013, 11:21 AM
 */

#ifndef G2SINKWRAPPER_IPP
#define	G2SINKWRAPPER_IPP

#include "g2logmessage.hpp"

namespace g2 {
  namespace internal {
    
    struct SinkWrapper {
      virtual ~SinkWrapper() { }
      virtual void send(const LogMessage& msg) = 0;
    };
  }
}

#endif	/* G2SINKWRAPPER_IPP */ 

