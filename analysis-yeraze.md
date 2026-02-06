# PR #559 Analysis: Calculate Relative Address and add to stack trace outputs

**Author:** Yeraze (Randall Hand)
**PR URL:** https://github.com/KjellKod/g3log/pull/559
**CI Status:** All checks passing (macOS, Ubuntu, Windows, Semgrep)

---

## 1. Does This Add Value?

**Yes, this PR adds meaningful value for Windows debugging scenarios.**

### Why It Matters

When a Windows application crashes in production, you typically have:
- The stack trace with absolute memory addresses
- The EXE and PDB files from that build

The problem: **Absolute addresses are useless** because ASLR (Address Space Layout Randomization) means the executable loads at different base addresses each time it runs.

This PR adds the **Relative Virtual Address (RVA)** to each stack frame. The RVA is calculated as:
```
RVA = absolute_address - module_base_address
```

With the RVA, you can use tools like `llvm-symbolizer` or `addr2line` to translate the address back to source code locations, even without the original crash environment.

### Practical Use Case

**Before this PR:**
```
stack dump [0]   SomeFunction
stack dump [1]   raise
stack dump [2]   abort
```

**After this PR:**
```
stack dump [0]   [A:0x04c50a20]   SomeFunction
stack dump [1]   [A:0x000c1989]   raise
stack dump [2]   [A:0x000a4ab1]   abort
```

Now a developer can run:
```bash
llvm-symbolizer --obj=myapp.exe 0x04c50a20
```
And get the exact file/line information from their PDB.

---

## 2. Is This a Quality PR?

**Mixed - functional but has room for improvement.**

### Positives
- Clean, minimal change (14 additions, 0 deletions)
- Uses appropriate Windows APIs (`SymGetModuleInfo64`)
- Follows existing code patterns
- Includes necessary header (`<iomanip>`)
- PR description is excellent with clear example output

### Concerns

#### Code Quality Issues

1. **Inconsistent formatting/indentation**
   - The new code uses 4-space indentation; surrounding code uses 3-space indentation
   - Tab vs space inconsistency with the output format string

2. **Output placement is suboptimal**
   - The RVA is inserted in the middle of the output string construction
   - Current output order: `[index] [RVA] [line info] [function name]`
   - The example in PR description shows: `[index] [RVA] [file/line] [function]`
   - But due to append order, actual output may vary based on symbol availability

3. **Missing error handling consideration**
   - If `SymGetModuleInfo64` fails, no RVA is printed (silent failure)
   - This is acceptable but could be documented

4. **Formatting checklist unchecked**
   - Author acknowledged they didn't run clang-format

---

## 3. Concerns with the Approach

### Minor Concerns

1. **Performance impact** - Adds one extra Windows API call (`SymGetModuleInfo64`) per stack frame
   - This happens during crash handling, so performance is not critical
   - The call is already within a section that calls `SymFromAddr` and `SymGetLineFromAddr64`
   - **Impact: Negligible**

2. **Output format choice `[A:0x...]`**
   - The `A:` prefix meaning isn't immediately obvious (Address? Absolute? Actually it's RVA which is Relative)
   - Could be `[RVA:0x...]` for clarity
   - **Impact: Minor, documentation would help**

3. **Fixed 8-digit hex width**
   - Uses `std::setw(8)` which is 32-bit width
   - On 64-bit Windows, RVAs could theoretically exceed this
   - In practice, module sizes rarely exceed 4GB, so this is fine
   - **Impact: None for realistic scenarios**

### No Major Concerns

- The code is safe - all Windows API calls are standard and well-documented
- No memory leaks or resource issues
- No threading concerns (already in a mutex-protected section)
- No breaking changes to existing functionality

---

## 4. How Can This Be Validated (Before/After)?

### Manual Validation Steps

1. **Build a test application on Windows with debug symbols**
   ```bash
   mkdir build && cd build
   cmake -DADD_G3LOG_UNIT_TEST=ON ..
   cmake --build . --config Debug
   ```

2. **Create a test program that triggers a crash with a known location**
   ```cpp
   #include <g3log/g3log.hpp>
   #include <g3log/logworker.hpp>

   void knownCrashLocation() {
       int* p = nullptr;
       *p = 42;  // Line X - note this line number
   }

   int main() {
       auto worker = g3::LogWorker::createLogWorker();
       g3::initializeLogging(worker.get());
       knownCrashLocation();
       return 0;
   }
   ```

3. **Run and capture the crash output**
   - Note the `[A:0x...]` value for the `knownCrashLocation` frame

4. **Verify with llvm-symbolizer or similar tool**
   ```bash
   llvm-symbolizer --obj=test_app.exe 0x<rva_value>
   ```
   - Should output the file path and line number matching where you put the crash

### Automated Validation

The existing CI passes, but it doesn't specifically test RVA output accuracy. See section 5 for unit testing recommendations.

---

## 5. Can/Should This Be Validated with Unit Testing?

### Current Test Coverage

The existing `test_crashhandler_windows.cpp` only tests:
- Exception ID to text conversion (`exceptionIdToText`)
- No tests for actual stack trace output content

### Should It Be Unit Tested?

**Yes, but with caveats.**

### Recommended Testing Approaches

#### Approach A: Output Format Verification (Recommended)

Add a test that verifies the stack dump output contains the expected format:

```cpp
// In test_crashhandler_windows.cpp

TEST(CrashHandler_Windows, StackDumpContainsRelativeAddress) {
    // Get a stackdump from current context
    std::string dump = stacktrace::stackdump();

    // Verify the RVA format appears in output
    // Pattern: [A:0x followed by 8 hex digits and ]
    std::regex rva_pattern(R"(\[A:0x[0-9a-fA-F]{8}\])");
    EXPECT_TRUE(std::regex_search(dump, rva_pattern))
        << "Stack dump should contain relative address in format [A:0x########]";
}
```

**Location:** `test_unit/test_crashhandler_windows.cpp`

#### Approach B: RVA Calculation Verification (More Complex)

Test that the calculated RVA can be used to recover the original function:

```cpp
TEST(CrashHandler_Windows, RVACanBeUsedForSymbolLookup) {
    // This would require:
    // 1. Capturing a stackdump
    // 2. Parsing out an RVA value
    // 3. Using SymFromAddr with (moduleBase + RVA)
    // 4. Verifying we get the same symbol name

    // More complex but validates the core functionality
}
```

#### Approach C: Integration Test

Create a test executable that:
1. Crashes intentionally
2. Captures the output
3. Parses out RVA values
4. Verifies against known symbol offsets from the PDB

**This is complex and may be overkill for this feature.**

### Testing Recommendation Summary

| Test Type | Effort | Value | Recommendation |
|-----------|--------|-------|----------------|
| Format regex check | Low | Medium | **Do this** |
| RVA calculation verification | Medium | High | Nice to have |
| Full integration with symbolizer | High | High | Overkill for this PR |

### Proposed Test Addition

```cpp
// Add to test_unit/test_crashhandler_windows.cpp

#include <regex>

TEST(CrashHandler_Windows, StackDumpFormat) {
    std::string dump = stacktrace::stackdump();

    // Should not be empty
    EXPECT_FALSE(dump.empty());

    // Should contain "stack dump" entries
    EXPECT_NE(dump.find("stack dump"), std::string::npos);

    // NEW: Should contain relative address format [A:0x########]
    std::regex rva_pattern(R"(\[A:0x[0-9a-fA-F]{8}\])");
    EXPECT_TRUE(std::regex_search(dump, rva_pattern))
        << "Expected relative address format [A:0x########] in output:\n" << dump;
}
```

---

## Summary

| Question | Answer |
|----------|--------|
| **Adds value?** | Yes - enables post-mortem debugging with RVA + symbolizer |
| **Quality PR?** | Functional but needs formatting cleanup |
| **Concerns?** | Minor: indentation, output format could be clearer |
| **Validation?** | Manual: trigger crash, verify RVA with symbolizer |
| **Unit testing?** | Yes - add regex test for output format |

### Recommended Actions Before Merge

1. **Required:** Run `clang-format -i src/stacktrace_windows.cpp` to fix indentation
2. **Optional:** Consider renaming `[A:...]` to `[RVA:...]` for clarity
3. **Optional:** Add basic unit test for output format verification

---

## Changes Made (Review Improvements)

The following changes have been applied to address the review concerns:

### 1. Formatting Fixed (Manual - clang-format unavailable)
- Changed 4-space indentation to 3-space to match project style
- Fixed brace formatting: `{ 0 }` → `{0}`

### 2. Renamed `[A:...]` to `[RVA:...]` with Documentation

**File:** `src/stacktrace_windows.cpp`

Added explanatory comment:
```cpp
// Calculate and output the Relative Virtual Address (RVA) for post-mortem debugging.
// The RVA can be used with tools like llvm-symbolizer to recover source locations
// from crash dumps, even with ASLR enabled.
// Reference: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#general-concepts
```

Output format changed from:
```
[A:0x04c50a20]
```
To:
```
[RVA:0x04c50a20]
```

### 3. Added Missing Header
- Added `#include <sstream>` for `std::stringstream`

### 4. Unit Tests Added

**File:** `test_unit/test_crashhandler_windows.cpp`

Three new tests added:

| Test Name | Purpose |
|-----------|---------|
| `StackDumpBasicStructure` | Verifies stackdump() returns non-empty output with "stack dump" entries |
| `StackDumpContainsRVA` | Verifies output contains `[RVA:0x########]` format using regex |
| `StackDumpFrameFormat` | Verifies frame entries match pattern `stack dump [N]` |

### 5. CI Testing

The existing CI workflow (`test_unit/Test.cmake` line 62) already includes `test_crashhandler_windows` in the Windows build:

```cmake
IF (MSVC OR MINGW)
   SET(OS_SPECIFIC_TEST test_crashhandler_windows)
ENDIF(MSVC OR MINGW)
```

The new tests will automatically run as part of the Windows CI on push/PR. They use `stacktrace::stackdump()` to capture a live stack trace and verify:
- The output is non-empty
- The RVA format appears in the output
- Frame entries are properly formatted

---

*Analysis generated: 2026-02-05*
*Review improvements applied: 2026-02-05*
