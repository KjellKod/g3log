/** ==========================================================================
 * 2014 by KjellKod.cc AND Robert Engeln.
 * The stacktrace code was given as a public domain dedication by Robert Engeln
 * It was originally published at: http://code-freeze.blogspot.com/2012/01/generating-stack-traces-from-c.html
 * It was (here) modified for g3log purposes.
 *
 * This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "stacktrace_windows.hpp"
#include <windows.h>
#include <map>

#pragma once
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#error "stacktrace_win.cpp used but not on a windows system"
#endif

#define g2_MAP_PAIR_STRINGIFY(x) {x, #x}

namespace {
const std::map<size_t, std::string> kExceptionsAsText = {
   g2_MAP_PAIR_STRINGIFY(EXCEPTION_ACCESS_VIOLATION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_ARRAY_BOUNDS_EXCEEDED)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_BREAKPOINT)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_DATATYPE_MISALIGNMENT)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_DENORMAL_OPERAND)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_DIVIDE_BY_ZERO)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INEXACT_RESULT)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INEXACT_RESULT)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_INVALID_OPERATION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_OVERFLOW)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_STACK_CHECK)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_FLT_UNDERFLOW)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_ILLEGAL_INSTRUCTION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_IN_PAGE_ERROR)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_INT_DIVIDE_BY_ZERO)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_INT_OVERFLOW)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_INVALID_DISPOSITION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_NONCONTINUABLE_EXCEPTION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_PRIV_INSTRUCTION)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_SINGLE_STEP)
   , g2_MAP_PAIR_STRINGIFY(EXCEPTION_STACK_OVERFLOW)
};
}


namespace stacktrace {
/// return the text description of a Windows exception code
/// From MSDN GetExceptionCode http://msdn.microsoft.com/en-us/library/windows/desktop/ms679356(v=vs.85).aspx
std::string exceptionIdToText(size_t id) {
   const auto iter = kExceptionsAsText.find(id);
   if ( iter == kExceptionsAsText.end()) {
      std::string unknown {"Unknown/" + std::to_string(id)};
      return unknown;
   }

   return iter->second;
}

} // stacktrace



