/* 
 * File:   g2moveoncopy.hpp
 * Author: kjell
 *
 * Created on November 11, 2013, 11:33 PM
 */

#pragma once
namespace g2 {

   // A straightforward technique to move around packaged_tasks.
   //  Instances of std::packaged_task are MoveConstructible and MoveAssignable, but
   //  not CopyConstructible or CopyAssignable. To put them in a std container they need
   //  to be wrapped and their internals "moved" when tried to be copied.

   template<typename Moveable>
   struct MoveOnCopy {
      mutable Moveable _move_only;

      explicit MoveOnCopy(Moveable&& m) : _move_only(std::move(m)) {}
      MoveOnCopy(MoveOnCopy const& t) : _move_only(std::move(t._move_only)) {}
      MoveOnCopy(MoveOnCopy&& t) : _move_only(std::move(t._move_only)) {}

      MoveOnCopy& operator=(MoveOnCopy const& other) {
         _move_only = std::move(other._move_only);
         return *this;
      }

      MoveOnCopy& operator=(MoveOnCopy&& other) {
         _move_only = std::move(other._move_only);
         return *this;
      }

      void operator()() { _move_only(); } 
      Moveable& get() { return _move_only; }
      Moveable release() { return std::move(_move_only); }
   };

} // g2