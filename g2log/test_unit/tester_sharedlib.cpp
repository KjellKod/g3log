
#include <g2log.hpp>
#include "tester_sharedlib.h"

struct RuntimeLoadedLib : public SomeLibrary {

   RuntimeLoadedLib() {
      LOG(INFO) << "Library was created";
      LOGF(INFO, "Ready for testing");
   }

   ~RuntimeLoadedLib() {
      LOG(DEBUG) << "Library destroyed";
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

