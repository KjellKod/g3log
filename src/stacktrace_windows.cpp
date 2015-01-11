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
#include <DbgHelp.h>
#include <map>
#include <memory>
#include <atomic>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <iostream> // to remove TODO
#include <sstream> // TODO REMOVE
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


// Using the given context, fill in all the stack frames.
// Which then later can be interpreted to human readable text
void captureStackTrace(CONTEXT *context, std::vector<uint64_t> &frame_pointers) {
   size_t machine_type = 0;
   STACKFRAME64 frame = {};
   frame.AddrPC.Mode = AddrModeFlat;
   frame.AddrFrame.Mode = AddrModeFlat;
   frame.AddrStack.Mode = AddrModeFlat;
#ifdef _M_X64
   frame.AddrPC.Offset = context->Rip;
   frame.AddrFrame.Offset = context->Rbp;
   frame.AddrStack.Offset = context->Rsp;
   machine_type = IMAGE_FILE_MACHINE_AMD64;
#else
   frame.AddrPC.Offset = context->Eip;
   frame.AddrPC.Offset = context->Ebp;
   frame.AddrPC.Offset = context->Esp;
   machine_type = IMAGE_FILE_MACHINE_I386;
#endif
   auto noErrors = TRUE; // Why, Oh Why have Microsoft redefined bool?
   for (size_t index = 0; noErrors && index < frame_pointers.size(); ++index)
   {
      noErrors = StackWalk64(machine_type,
                             GetCurrentProcess(),
                             GetCurrentThread(),
                             &frame,
                             context,
                             NULL,
                             SymFunctionTableAccess64,
                             SymGetModuleBase64,
                             NULL);
      if (noErrors) {
         frame_pointers[index] = frame.AddrPC.Offset;
      }
   }
}

// extract readable text from a given stack frame. All thanks to
// using SymFromAddr and SymGetLineFromAddr64 with the stack pointer
std::string getSymbolInformation(size_t index, std::vector<uint64_t> &frame_pointers) {
   auto addr = frame_pointers[index];
   std::string frame_dump = "stack dump [" + std::to_string(index) + "]\t";


   DWORD64 displacement64;
   DWORD displacement;
   char symbol_buffer[sizeof(SYMBOL_INFO) + 256];
   SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(symbol_buffer);
   symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
   symbol->MaxNameLen = MAX_SYM_NAME;

   IMAGEHLP_LINE64 line;
   line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
   std::string lineInformation;
   std::string callInformation;
   if (SymFromAddr(GetCurrentProcess(),addr, &displacement64,symbol)){
      callInformation.append(" ").append({symbol->Name, symbol->NameLen});
      if (SymGetLineFromAddr64(GetCurrentProcess(), addr,&displacement,&line)){
         lineInformation.append("\t").append(line.FileName).append(" L: ");
         lineInformation.append(std::to_string(line.LineNumber));
      }
   }
   frame_dump.append(lineInformation).append(callInformation);
   return frame_dump;
}


// Retrieves all the symbols for the stack frames, fills them witin a text representation and returns it
std::string convertFramesToText(std::vector<uint64_t> &frame_pointers) {
   std::string dump; // slightly more efficient than ostringstream
   const size_t size = frame_pointers.size();
   for (size_t index = 0; index < size && frame_pointers[index]; ++index) {
      dump += getSymbolInformation(index, frame_pointers);
      dump += "\n";
   }
   return dump;
}
} // anonymou


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


/// helper function: retrieve stackdump from no excisting exception pointer
std::string stackdump() {
   CONTEXT current_context;
   memset(&current_context, 0, sizeof(CONTEXT));
   RtlCaptureContext(&current_context);
   return stackdump(&current_context);
}

/// helper function: retrieve stackdump, starting from an exception pointer
std::string stackdump(EXCEPTION_POINTERS *info) {
   auto context = info->ContextRecord;
   return stackdump(context);

}

/// main stackdump function. retrieve stackdump, from the given context
std::string stackdump(CONTEXT *context) {
   {
      static std::atomic<size_t> recursiveCounter = 0;
      ++recursiveCounter;
      assert(recursiveCounter.load() == 1 && "Never allow recursive crashes");
   }

   const BOOL kLoadSymModules = TRUE;
   const auto initialized = SymInitialize(GetCurrentProcess(), nullptr, kLoadSymModules);
   std::shared_ptr<void> RaiiSymCleaner(nullptr, [&](void *) {
      if (initialized) {
         SymCleanup(GetCurrentProcess());
      }
   }); // Raii sym cleanup


   const size_t kmax_frame_dump_size = 64;
   std::vector<uint64_t>  frame_pointers(kmax_frame_dump_size); // C++11: size set and values are zeroed
   captureStackTrace(context, frame_pointers);
   return convertFramesToText(frame_pointers);
}




} // stacktrace



