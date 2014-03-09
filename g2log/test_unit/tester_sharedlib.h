#pragma once

struct SomeLibrary {

   SomeLibrary() {};

   virtual ~SomeLibrary() {};
   virtual void action() = 0;
};

class LibraryFactory {
public:
   virtual SomeLibrary* CreateLibrary() = 0;
};
