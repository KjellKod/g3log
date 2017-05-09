/** ==========================================================================
 * 2014 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * 
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/


#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "tester_sharedlib.h"

#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
#define DEBUG_NAME DBUG
#else
#define DEBUG_NAME DEBUG
#endif

struct RuntimeLoadedLib : public SomeLibrary {

   RuntimeLoadedLib() {
      LOG(INFO) << "Library was created";
      LOGF(INFO, "Ready for testing");
   }

   ~RuntimeLoadedLib() {
      LOG(DEBUG_NAME) << "Library destroyed";
   }

   void action() {
      LOG(WARNING) << "Action, action, action. Safe for LOG calls by runtime dynamically loaded libraries";
   }
};

struct RealLibraryFactory : public LibraryFactory {
   SomeLibrary* CreateLibrary() {
      return new RuntimeLoadedLib;
   }
};

RealLibraryFactory testRealFactory;

