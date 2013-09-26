/** ==========================================================================
 * 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================
 *
 * Example of a Active Object, using C++11 std::thread mechanisms to make it
 * safe for thread communication.
 *
 * This was originally published at http://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
 * and inspired from Herb Sutter's C++11 Active Object
 * http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads
 *
 * Last update 2013-09-25 by Kjell Hedstrom,
 * e-mail: hedstrom at kjellkod dot cc
 * linkedin: http://linkedin.com/se/kjellkod */

#pragma once

#include <thread>
#include <functional>
#include <memory>

#include "shared_queue.h"

namespace kjellkod {
typedef std::function<void() > Callback;

class Active {
private:
   Active(); // Construction ONLY through factory createActive();
   void run();

   shared_queue<Callback> mq_;
   std::thread thd_;
   bool done_; // finished flag : set by ~Active


public:
   virtual ~Active();
   void send(Callback msg_);
   static std::unique_ptr<Active> createActive();

   Active(const Active&) = delete;
   Active& operator=(const Active&) = delete;
};

} // kjellkod
