/** ==========================================================================
 * 2014 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include <gtest/gtest.h>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <windows.h>
#include <regex>
#include <string>
#include "g3log/stacktrace_windows.hpp"


TEST(CrashHandler_Windows, ExceptionType) {
   EXPECT_EQ(stacktrace::exceptionIdToText(123), "UNKNOWN EXCEPTION:123");
   EXPECT_EQ(stacktrace::exceptionIdToText(1), "UNKNOWN EXCEPTION:1");

   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_ACCESS_VIOLATION), "EXCEPTION_ACCESS_VIOLATION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_ARRAY_BOUNDS_EXCEEDED), "EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_BREAKPOINT), "EXCEPTION_BREAKPOINT");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_DATATYPE_MISALIGNMENT), "EXCEPTION_DATATYPE_MISALIGNMENT");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_DENORMAL_OPERAND), "EXCEPTION_FLT_DENORMAL_OPERAND");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_DIVIDE_BY_ZERO), "EXCEPTION_FLT_DIVIDE_BY_ZERO");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_INEXACT_RESULT), "EXCEPTION_FLT_INEXACT_RESULT");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_INEXACT_RESULT), "EXCEPTION_FLT_INEXACT_RESULT");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_INVALID_OPERATION), "EXCEPTION_FLT_INVALID_OPERATION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_OVERFLOW), "EXCEPTION_FLT_OVERFLOW");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_STACK_CHECK), "EXCEPTION_FLT_STACK_CHECK");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_FLT_UNDERFLOW), "EXCEPTION_FLT_UNDERFLOW");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_ILLEGAL_INSTRUCTION), "EXCEPTION_ILLEGAL_INSTRUCTION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_IN_PAGE_ERROR), "EXCEPTION_IN_PAGE_ERROR");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_INT_DIVIDE_BY_ZERO), "EXCEPTION_INT_DIVIDE_BY_ZERO");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_INT_OVERFLOW), "EXCEPTION_INT_OVERFLOW");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_INVALID_DISPOSITION), "EXCEPTION_INVALID_DISPOSITION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_NONCONTINUABLE_EXCEPTION), "EXCEPTION_NONCONTINUABLE_EXCEPTION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_PRIV_INSTRUCTION), "EXCEPTION_PRIV_INSTRUCTION");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_SINGLE_STEP), "EXCEPTION_SINGLE_STEP");
   EXPECT_EQ(stacktrace::exceptionIdToText(EXCEPTION_STACK_OVERFLOW), "EXCEPTION_STACK_OVERFLOW");
}

// Test that stackdump() produces non-empty output with expected structure
TEST(CrashHandler_Windows, StackDumpBasicStructure) {
   std::string dump = stacktrace::stackdump();

   // Stack dump should not be empty
   EXPECT_FALSE(dump.empty()) << "stackdump() returned empty string";

   // Should contain "stack dump" entries
   EXPECT_NE(dump.find("stack dump"), std::string::npos)
      << "Stack dump should contain 'stack dump' entries. Got:\n"
      << dump;
}

// Test that stackdump() contains the Relative Virtual Address (RVA) format
// RVA enables post-mortem debugging with tools like llvm-symbolizer
// Reference: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
TEST(CrashHandler_Windows, StackDumpContainsRVA) {
   std::string dump = stacktrace::stackdump();

   // Should contain RVA format: [RVA:0x followed by 8 hex digits and ]
   // Example: [RVA:0x04c50a20]
   std::regex rva_pattern(R"(\[RVA:0x[0-9a-fA-F]{8}\])");
   EXPECT_TRUE(std::regex_search(dump, rva_pattern))
      << "Stack dump should contain relative address in format [RVA:0x########].\n"
      << "This is needed for post-mortem debugging with llvm-symbolizer.\n"
      << "Got:\n"
      << dump;
}

// Test that each stack frame line has expected components
TEST(CrashHandler_Windows, StackDumpFrameFormat) {
   std::string dump = stacktrace::stackdump();

   // Each frame should have format: "stack dump [N]" where N is frame index
   std::regex frame_pattern(R"(stack dump \[\d+\])");
   EXPECT_TRUE(std::regex_search(dump, frame_pattern))
      << "Stack dump should contain frame entries like 'stack dump [0]'. Got:\n"
      << dump;
}

#endif  // defined WIN32
