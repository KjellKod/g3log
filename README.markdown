# G3log : Asynchronous logger with Dynamic Sinks


## EXAMPLE USAGE
#### Optional to use either streaming or printf-like syntax
```
LOG(INFO) << "streaming API is as easy as ABC or " << 123;

LOGF(WARNING, "Printf-style syntax is also %s", "available");
```

# Content
* [What G3log is](#what-g3log-is)
  * [Conditional Logging](#conditional-logging)
  * [Design by Contract](#design-by-contract)
  * [Detailed API documentation](API.markdown)
  * [Benefits with g3log](#benefits-with-g3log)
* [G3log with sinks](#g3log-with-sinks)
  * [Crazy simple to create a custom sink](#crazy-simple)
* [Code Examples](#code-examples)
  * [Custom Sink Walkthrough](#custom-sink)
  * [Default File Logger](#default-file-logger)
* [Building G3log](#building-g3log)
  * [Prerequisites](#prerequisites)
  * [Configuration Options](#configuration)
  * [Build Commands](#build-commands)
  * [Installation](#installation)
  * [Packaging](#packaging)
  * [Testing](#testing)
  * [CMake Module](#cmake-module)
* Overview of the [API description](#overview-api-description)
* [Performance](#performance)
* [Feedback](#feedback)
* [Say Thanks](#say-thanks)


## <a name="what-g3log-is">What G3Log is</a>

* ***G3log*** is the acting name for the third version of g2log and it stands for **g3log with dynamic sinks**
* G3log is an asynchronous, "crash-safe" logger. You can read more about it here [[g2log version]](
http://www.codeproject.com/Articles/288827/g2log-An-efficient-asynchronous-logger-using-Cplus)
* You can choose to use the default log receiver which saves all LOG calls to file, **or** you can choose to use your own custom made log receiver(s), **or** both, **or** as many sinks as you need.



#### <a name="#conditional-logging">Conditional logging</a>
    int less = 1; int more = 2
    LOG_IF(INFO, (less<more)) <<"If [true], then this text will be logged";

    // or with printf-like syntax
    LOGF_IF(INFO, (less<more), "if %d<%d then this text will be logged", less,more);



#### <a name="design-by-contract">Design-by-Contract</a>
*CHECK(false)* will trigger a "fatal" message. It will be logged, and then the
application will exit.

```
CHECK(less != more); // not FATAL
CHECK(less > more) << "CHECK(false) triggers a FATAL message";
```




## Detailed API documentation
Please look at [API.markdown](API.markdown) for detailed API documentation


## <a name="benefits-with-g3log">Benefits you get when using G3log</a>
1. Easy to use, clean syntax and a blazing fast logger.

2. All the slow log I/O disk access is done in a background thread. This ensures that the LOG caller can immediately continue with other tasks and do not have to wait for the LOG call to finish.

3. G3log provides logging, Design-by-Contract [#CHECK], and flush of log to file at
 shutdown. Buffered logs will be written to the sink before the application shuts down.

4. It is thread safe, so using it from multiple threads is completely fine.

5. It is *CRASH SAFE*. It will save the made logs to the sink before it shuts down.
The logger will catch certain fatal events *(Linux/OSX: signals, Windows: fatal OS exceptions and signals)* , so if your application  crashes due to, say a segmentation fault, *SIGSEGV*,  it will  log and save the crash and all previously buffered log  entries before exiting.


6. It is cross platform. Tested and used by me or by clients on OSX, Windows, Ubuntu, CentOS

7. G3log and G2log are used worldwide in commercial products as well as hobby projects. G2log is used since early 2011.

8. The code is given for free as public domain. This gives the option to change, use, and do whatever with it, no strings attached.

9. Two versions of g3log exists.
    * This version: *[g3log](https://github.com/KjellKod/g3log)* : which is made to facilitate  easy adding of custom log receivers.  Its tested on at least the following platforms with Linux(Clang/gcc), Windows (mingw, visual studio 2013). My recommendation is to go with g3log if you have full C++14 support (C++11 support up to version: https://github.com/KjellKod/g3log/releases/tag/1.3.1).
    * *[g2log](https://bitbucket.org/KjellKod/g2log)*: The original. Simple, easy to modify and with the most OS support. Clients use g2log on environments such as OSX/Clang, Ubuntu, CentOS, Windows/mingw, Windows/Visual Studio.  The focus on g2log is "slow to change" and compiler support. Only well, time tested, features from g3log will make it into g2log. Currently there is not active development or support on g2log but feel free to shoot me a question if you need assistance.




# <a name="g3log-with-sinks">G3log with sinks</a>
[Sinks](http://en.wikipedia.org/wiki/Sink_(computing)) are receivers of LOG calls. G3log comes with a default sink (*the same as G3log uses*) that can be used to save log to file.  A sink can be of *any* class type without restrictions as long as it can either receive a LOG message as a  *std::string* **or** as a *g3::LogMessageMover*.

The *std::string* comes pre-formatted. The *g3::LogMessageMover* is a wrapped struct that contains the raw data for custom handling in your own sink.

A sink is *owned* by the G3log and is added to the logger inside a ```std::unique_ptr```.  The sink can be called though its public API through a *handler* which will asynchronously forward the call to the receiving sink.

It is <a name="crazy-simple">crazy simple to create a custom sink</a>. This example show what is needed to make a custom sink that is using custom log formatting but only using that
for adding color to the default log formatting. The sink forwards the colored log to cout


```cpp
// in file Customsink.hpp
#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

struct CustomSink {

// Linux xterm color
// http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  enum FG_Color {YELLOW = 33, RED = 31, GREEN=32, WHITE = 97};

  FG_Color GetColor(const LEVELS level) const {
     if (level.value == WARNING.value) { return YELLOW; }
     if (level.value == DEBUG.value) { return GREEN; }
     if (g3::internal::wasFatal(level)) { return RED; }

     return WHITE;
  }

  void ReceiveLogMessage(g3::LogMessageMover logEntry) {
     auto level = logEntry.get()._level;
     auto color = GetColor(level);

     std::cout << "\033[" << color << "m"
       << logEntry.get().toString() << "\033[m" << std::endl;
  }
};

// in main.cpp, main() function

auto sinkHandle = logworker->addSink(std::make_unique<CustomSink>(),
                                     &CustomSink::ReceiveLogMessage);

```


**More sinks** can be found in the repository **[github.com/KjellKod/g3sinks](https://github.com/KjellKod/g3sinks)**.


# <a name="code-examples">Code Examples</a>
Example usage where a <a name="custom-sink">custom sink</a> is added. A function is called though the sink handler to the actual sink object.
```cpp
// main.cpp
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <memory>

#include "CustomSink.h"

int main(int argc, char**argv) {
   using namespace g3;
   std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
   auto sinkHandle = logworker->addSink(std::make_unique<CustomSink>(),
                                          &CustomSink::ReceiveLogMessage);

   // initialize the logger before it can receive LOG calls
   initializeLogging(logworker.get());
   LOG(WARNING) << "This log call, may or may not happend before"
                << "the sinkHandle->call below";


   // You can call in a thread safe manner public functions on your sink
   // The call is asynchronously executed on your custom sink.
   std::future<void> received = sinkHandle->call(&CustomSink::Foo,
                                                 param1, param2);

   // If the LogWorker is initialized then at scope exit the g3::internal::shutDownLogging() will be called.
   // This is important since it protects from LOG calls from static or other entities that will go out of
   // scope at a later time.
   //
   // It can also be called manually:
   g3::internal::shutDownLogging();
}


// some_file.cpp : To show how easy it is to get the logger to work
// in other parts of your software

#include <g3log/g3log.hpp>

void SomeFunction() {
   ...
   LOG(INFO) << "Hello World";
}
```

Example usage where a the <a name="default-file-logger">default file logger</a> is used **and** a custom sink is added
```cpp
// main.cpp
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <memory>

#include "CustomSink.h"

int main(int argc, char**argv) {
   using namespace g3;
   auto worker = LogWorker::createLogWorker();
   auto defaultHandler = worker->addDefaultLogger(argv[0],
                                                 path_to_log_file);

   // logger is initialized
   g3::initializeLogging(worker.get());

   LOG(DEBUG) << "Make log call, then add another sink";

   worker->addSink(std::make_unique<CustomSink>(),
                                  &CustomSink::ReceiveLogMessage);

   ...
}
```



# <a name="building-g3log">Building G3log</a>

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

g3log comes with a CMake module. Once installed, it can be found under `${CMAKE_INSTALL_PREFIX}/lib/cmake/g3logger`. Users can use g3log in a CMake-based project this way:

```
find_package(g3logger CONFIG REQUIRED)
target_link_libraries(main PRIVATE g3logger)
```

*Note:* The CMake package name here is `g3logger`, not `g3log`.

To make sure that CMake can find g3log(or g3logger), you also need to tell CMake where to search for it:
```
cmake .. -DCMAKE_PREFIX_PATH=<g3log's install prefix>
```

# Overview of the <a name="overview-api-description">API description</a>
Most of the API that you need for using g3log is described in this readme. For more API documentation and examples please continue to read the [API readme](API.markdown). Examples of what you will find here are:

* Sink creation and utilization
* Logging levels
  * disable/enabled levels at runtime
  * custom logging levels
* Fatal handling
  * custom fatal handling
  * pre fatal hook
  * override of signal handling
  * disable fatal handling
* LOG calls
* CHECK calls


# <a name="performance">Performance</a>
G3log aims to keep all background logging to sinks with as little log overhead as possible to the logging sink and with as small "worst case latency" as possible. For this reason g3log is a good logger for many systems that deal with critical tasks. Depending on platform the average logging overhead will differ. On my 2010 laptop the average call, when doing extreme performance testing, will be about ~2 us.

The worst case latency is kept stable with no extreme peaks, in spite of any sudden extreme pressure.  I have a blog post regarding comparing worst case latency for g3log and other loggers which might be of interest.
You can find it here: https://kjellkod.wordpress.com/2015/06/30/the-worlds-fastest-logger-vs-g3log/

# <a name="feedback">Feedback</a>
If you like this logger (or not) it would be nice with some feedback. That way I can improve g3log and g2log and it is also nice to see if someone is using it.

 If you have ANY questions or problems please do not hesitate in contacting me on my blog
http://kjellkod.wordpress.com/2011/11/17/kjellkods-g2log-vs-googles-glog-are-asynchronous-loggers-taking-over  
or at ```Hedstrom at KjellKod dot cc```

# <a name="say-thanks">Say Thanks</a>
This logger is available for free and all of its source code is public domain.  A great way of saying thanks is to send a donation. It would go a long way not only to show your support but also to boost continued development.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/g3log/25)

* $5 for a cup of coffee
* $10 for pizza
* $25 for a lunch or two
* $100 for a date night with my wife (which buys family credit for evening coding)
* $$$ for upgrading my development environment
* $$$$ :)

Cheers

Kjell *(a.k.a. KjellKod)*
