[introduction](index.md) | [detailed information](g3log.md) | [**Configure & Build**](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)

# <a name="building-g3log">Configure, build, package, install and test g3log</a>

## Example Project with g3log
An example project integration of g3log, both statially and dynamically built can be found at [g3log_example_integration](https://github.com/KjellKod/g3log_example_integration/blob/master/README.md)


## Building it standalone to try out is as easy as: 
```
git clone https://github.com/KjellKod/g3log
cd g3log
mkdir build
cd build
```

## <a name="prerequisites">Prerequisites</a>
Assume you have got your shiny C++14 compiler installed, you also need these tools to build g3log from source:
- CMake (*Required*)

  g3log uses CMake as a one-stop solution for configuring, building, installing, packaging and testing on Windows, Linux and OSX.

- Git (*Optional but Recommended*)

  When building g3log it uses git to calculate the software version from the commit history of this repository. If you don't want that, or your setup does not have access to git, or you download g3log source archive from the GitHub Releases page so that you do not have the commit history downloaded, you can instead pass in the version as part of the CMake build arguments. See this [_issue_](https://github.com/KjellKod/g3log/issues/311#issuecomment-488829282) for more information.
  ```
  cmake -DVERSION=1.3.2  ..
  ```

## <a name="configuration">Configuration Options</a>
g3log provides following CMake options (and default values):
```
$ cmake -LAH # List non-advanced cached variables. See `cmake --help` for more details.

...

// Fatal (fatal-crashes/contract) examples
ADD_FATAL_EXAMPLE:BOOL=ON

// g3log performance test
ADD_G3LOG_BENCH_PERFORMANCE:BOOL=OFF

// g3log unit tests
ADD_G3LOG_UNIT_TEST:BOOL=OFF

// Use DBUG logging level instead of DEBUG.
// By default DEBUG is the debugging level
CHANGE_G3LOG_DEBUG_TO_DBUG:BOOL=OFF

// Specifies the build type on single-configuration generators.
// Possible values are empty, Debug, Release, RelWithDebInfo, MinSizeRel, …
CMAKE_BUILD_TYPE:STRING=

// Install path prefix, prepended onto install directories.
// This variable defaults to /usr/local on UNIX
// and c:/Program Files/${PROJECT_NAME} on Windows.
CMAKE_INSTALL_PREFIX:PATH=

// The prefix used in the built package.
// On Linux, if this option is not set:
// 1) If CMAKE_INSTALL_PREFIX is given, then it will be
//    set with the value of CMAKE_INSTALL_PREFIX by g3log.
// 2) Otherwise, it will be set as /usr/local by g3log.
CPACK_PACKAGING_INSTALL_PREFIX:PATH=

// Enable Visual Studio break point when receiving a fatal exception.
// In __DEBUG mode only
DEBUG_BREAK_AT_FATAL_SIGNAL:BOOL=OFF

// Vectored exception / crash handling with improved stack trace
ENABLE_FATAL_SIGNALHANDLING:BOOL=ON

// Vectored exception / crash handling with improved stack trace
ENABLE_VECTORED_EXCEPTIONHANDLING:BOOL=ON

// iOS version of library.
G3_IOS_LIB:BOOL=OFF

// Log full filename
G3_LOG_FULL_FILENAME:BOOL=OFF

// Build shared library
G3_SHARED_LIB:BOOL=ON

// Build shared runtime library MSVC
G3_SHARED_RUNTIME:BOOL=ON

// Turn ON/OFF log levels.
// An disabled level will not push logs of that level to the sink.
// By default dynamic logging is disabled
USE_DYNAMIC_LOGGING_LEVELS:BOOL=OFF

// Use dynamic memory for message buffer during log capturing
USE_G3_DYNAMIC_MAX_MESSAGE_SIZE:BOOL=OFF

...
```
For additional option context and comments please also see [Options.cmake](https://github.com/KjellKod/g3log/blob/master/Options.cmake)

If you want to leave everything as it was, then you should:
```
cmake ..
```
You may also specify one or more of those options listed above from the command line.
For example, on Windows:
```
cmake .. -G "Visual Studio 15 2017"
         -DG3_SHARED_LIB=OFF
         -DCMAKE_INSTALL_PREFIX=C:/g3log
         -DADD_G3LOG_UNIT_TEST=ON
         -DADD_FATAL_EXAMPLE=OFF
```
will use a Visual Studio 2017 solution generator, build g3log as a static library, headers and libraries will be installed to `C:\g3log` when installed from source, enable unit testing, but do not build fatal example.

MinGW users on Windows may find they should use a different generator:
```
cmake .. -G "MinGW Makefiles"
```

By default, headers and libraries will be installed to `/usr/local` on Linux when installed from build tree via `make install`. You may overwrite it by:
```
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
```
This will install g3log to `/usr` instead of `/usr/local`.

Linux/OSX package maintainers may be interested in the `CPACK_PACKAGING_INSTALL_PREFIX`. For example:
```
cmake .. -DCPACK_PACKAGING_INSTALL_PREFIX=/usr/local
```

## <a name="build-commands">Build Commands</a>
Once the configuration is done, you may build g3log with:
```
# Suppose you are still in the `build` directory. I won't repeat it anymore!
cmake --build . --config Release
```
You may also build it with a system-specific way.

On Linux, OSX and MinGW:
```
make
```
On Windows:
```
msbuild g3log.sln /p:Configuration=Release
```
Windows users can also open the generated Visual Studio solution file and build it happily.

## <a name="installing">Installation</a>
Install from source in a CMake way:
```
cmake --build . --target install
```
Linux users may also use:
```
sudo make install
```
You may also create a package first and install g3log with it. See the next section.

## <a name=packaging>Packaging</a>
A CMake way:
```
cmake --build . --config Release --target package
```
or
```
cpack -C Release
```
if the whole library has been built in the previous step.
It will generate a ZIP package on Windows, and a DEB package on Linux.

Linux users may also use a Linux way:
```
make package
```

If you want to use a different package generator, you should specify a `-G` option.

On Windows:
```
cpack -C Release -G NSIS;7Z
```
this will create a installable NSIS package and a 7z package.

*Note:* To use the NSIS generator, you should install [```NSIS```](https://nsis.sourceforge.io/Download) first.

On Linux:
```
cpack -C Release -G TGZ
```
this will create a .tar.gz archive for you.

Once done, you may install or uncompress the package file to the target machine. For example, on Debian or Ubuntu:
```
sudo dpkg -i g3log-<version>-Linux.deb
```
will install the g3log library to `CPACK_PACKAGING_INSTALL_PREFIX`.

## <a name="testing">Testing</a>

By default, tests will not be built. To enable unit testing, you should turn on `ADD_G3LOG_UNIT_TEST`.

Suppose the build process has completed, then you can run the tests with:
```
ctest -C Release
```
or:
```
make test
```
for Linux users.
or for a detailed gtest output of all the tests:
```
cd build;
../scripts/runAllTests.sh
```

## <a name="cmake-module">CMake module</a>

g3log comes with a CMake module. Once installed, it can be found under `${CMAKE_INSTALL_PREFIX}/lib/cmake/g3log`. Users can use g3log in a CMake-based project this way:

```
find_package(g3log CONFIG REQUIRED)
target_link_libraries(main PRIVATE g3log)
```

To make sure that CMake can find g3log, you also need to tell CMake where to search for it:
```
cmake .. -DCMAKE_PREFIX_PATH=<g3log's install prefix>
```

## <a name="build_options">Build Options</a>
  The build options are defined in the file [Options.cmake](https://github.com/KjellKod/g3log/blob/master/Build.cmake)

  build options are generated and saved to a header file. This avoid having to set the define options in the client source code

[introduction](index.md) | [detailed information](g3log.md) | [**Configure & Build**](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)