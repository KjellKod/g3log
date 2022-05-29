[introduction](index.md) | [**detailed information**](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)


# How to use g3log
G3log is an asynchronous logger with dynamic sinks


## Example USAGE
#### Optional to use either streaming or printf-like syntax
```
LOG(INFO) << "streaming API is as easy as ABC or " << 123;

LOGF(WARNING, "Printf-style syntax is also %s", "available");
```

## <a name="what-g3log-is">What g3Log is</a>

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




### Detailed API documentation
Please look at [API.md](API.md) for detailed API documentation


## <a name="benefits-with-g3log">Benefits you get when using g3log</a>
1. Easy to use, clean syntax and a blazing fast logger.

2. All the slow log I/O disk access is done in a background thread. This ensures that the LOG caller can immediately continue with other tasks and do not have to wait for the LOG call to finish.

3. G3log provides logging, Design-by-Contract [#CHECK], and flush of log to file at
 shutdown. Buffered logs will be written to the sink before the application shuts down.

4. It is thread safe, so using it from multiple threads is completely fine.

5. It is *CRASH SAFE*. It will save the made logs to the sink before it shuts down.
The logger will catch certain fatal events *(Linux/OSX: signals, Windows: fatal OS exceptions and signals)* , so if your application  crashes due to, say a segmentation fault, *SIGSEGV*,  it will  log and save the crash and all previously buffered log  entries before exiting.


6. It is cross platform. Tested and used by me or by clients on OSX, Windows, Ubuntu, CentOS

7. G3log and G2log are used worldwide in commercial products as well as hobby projects. G2log was introduced in early 2011 and is now retired.

8. The code is given for free as public domain. This gives the option to change, use, and do whatever with it, no strings attached.

9. *[g3log](https://github.com/KjellKod/g3log)* : is made to facilitate  easy adding of custom log receivers.  Its tested on at least the following platforms with **Linux**(Clang/gcc), **Windows** (mingw, visual studio) and **OSX**. My recommendation is to go with g3log if you have full C++14 support (C++11 support up to version: https://github.com/KjellKod/g3log/releases/tag/1.3.1).


## <a name="g3log-with-sinks">G3log with sinks</a>
[Sinks](http://en.wikipedia.org/wiki/Sink_(computing)) are receivers of LOG calls. G3log comes with a default sink (*the same as g3log uses*) that can be used to save log to file.  A sink can be of *any* class type without restrictions as long as it can either receive a LOG message as a  *std::string* **or** as a *g3::LogMessageMover*.

The *std::string* comes pre-formatted. The *g3::LogMessageMover* is a wrapped struct that contains the raw data for custom handling in your own sink.

A sink is *owned* by the g3log and is added to the logger inside a ```std::unique_ptr```.  The sink can be called though its public API through a *handler* which will asynchronously forward the call to the receiving sink.

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

## Adding and Removing Sinks
You can safely remove and add sinks during the running of your program. 

**Keep in mind**

- *Initialization of the logger should happen before you have started any other threads that may call the logger.* 
- *Destruction of the logger (RAII concept) should happen AFTER shutdown of other threads that are calling the logger.*

**Adding Sinks**
```cpp
   auto sinkHandle1 = logworker->addSink(std::make_unique<CustomSink>(),
                                          &CustomSink::ReceiveLogMessage);
   auto sinkHandle2 = logworker->addDefaultLogger(argv[0],
                                              path_to_log_file);
   logworker->removeSink(std::move(sinkHandle1)); // this will in a thread-safe manner remove the sinkHandle1
   logworker->removeAllSinks(); // this will in a thread-safe manner remove any sinks. 
```


**More sinks** can be found in the repository **[github.com/KjellKod/g3sinks](https://github.com/KjellKod/g3sinks)**.


## <a name="code-examples">Code Examples</a>
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

[introduction](index.md) | [**detailed information**](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)
