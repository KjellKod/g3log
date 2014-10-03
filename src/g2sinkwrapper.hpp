/** ==========================================================================
* 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

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

