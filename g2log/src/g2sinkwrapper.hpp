/* 
 * File:   g2sinkwrapper.hpp
 * Author: kjell
 *
 * Created on July 21, 2013, 11:21 AM
 */

#ifndef G2SINKWRAPPER_HPP
#define	G2SINKWRAPPER_HPP

#include "g2logmessage.hpp"

namespace g2 {
  namespace internal {
    
    struct SinkWrapper {
      virtual ~SinkWrapper() { }
      virtual void send(LogMessageMover msg) = 0;
    };
  }
}

#endif	/* G2SINKWRAPPER_IPP */ 

