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


#pragma once
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#error "stacktrace_win.cpp used but not on a windows system"
#endif

#include <string>
#include <windows.h>

namespace stacktrace {
/// return the text description of a Windows exception code
std::string exceptionIdToText(size_t id);

/// helper function: retrieve stackdump from no excisting exception pointer
std::string stackdump(); 

/// helper function: retrieve stackdump, starting from an exception pointer
std::string stackdump(EXCEPTION_POINTERS* info);

/// main stackdump function. retrieve stackdump, from the given context
std::string stackdump(CONTEXT* context);

} // stacktrace
