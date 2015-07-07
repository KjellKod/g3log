# G3log : Asynchronous logger with Dynamic Sinks


## EXAMPLE USAGE
#### Optional to use either streaming or printf-like syntax
```
LOG(INFO) << "streaming API is as easy as ABC or " << 123;

LOGF(WARNING, "Printf-style syntax is also %s", "available");
```



#### Conditional logging
    int less = 1; int more = 2
    LOG_IF(INFO, (less<more)) <<"If [true], then this text will be logged";
    
    // or with printf-like syntax
    LOGF_IF(INFO, (less<more), "if %d<%d then this text will be logged", less,more);



#### Design-by-Contract
*CHECK(false)* will trigger a "fatal" message. It will be logged, and then the 
application will exit.

```
CHECK(less != more); // not FATAL
CHECK(less > more) << "CHECK(false) triggers a FATAL message";
```


## What G3Log is: 
* ***G3log*** is the acting name for the third version of g2log and it stands for **g2log with dynamic sinks**
* G3log is an asynchronous, "crash-safe" logger. You can read more about it here [[g2log version]](
http://www.codeproject.com/Articles/288827/g2log-An-efficient-asynchronous-logger-using-Cplus)
* You can choose to use the default log receiver which saves all LOG calls to file, **or** you can choose to use your own custom made log receiver(s), **or** both, **or** as many sinks as you need.




## Benefits you get when using G3log ##
1. Easy to use, clean syntax and a blazing fast logger. 

2. All the slow log I/O disk access is done in a background thread. This ensures that the LOG caller can immediately continue with other tasks and do not have to wait for the LOG call to finish.

3. G3log provides logging, Design-by-Contract [#CHECK], and flush of log to file at
 shutdown. Buffered logs will be written to the sink before the application shuts down.

4. It is thread safe, so using it from multiple threads is completely fine. 

5. It is *CRASH SAFE*. It will save the made logs to the sink before it shuts down. 
The logger will catch certain fatal events *(Linux/OSX: signals, Windows: fatal OS exceptions and signals)* , so if your application  crashes due to, say a segmentation fault, *SIGSEGV*,  it will  log and save the crash and all previously buffered log  entries before exiting.

 
6. It is cross platform. Tested and used by me or by clients on OSX, Windows, Ubuntu, CentOS

7. G3log and G2log is used world wide in commercial products as well as hobby projects. G2log is used since early 2011.

8. The code is given for free as public domain. This gives the option to change, use, and do whatever with it, no strings attached.

9. Two versions of g2log exist that are under active development.
    * This version: *[g3log](https://bitbucket.org/KjellKod/g3log)* : which is made to facilitate  easy adding of custom log receivers.  Its tested on at least the following platforms with Linux(Clang/gcc), Windows (mingw, visual studio 2013). My recommendation is to go with g3log if you have full C++11 support. 
    * *[g2log](https://bitbucket.org/KjellKod/g2log)*: The original. Simple, easy to modify and with the most OS support. Clients use g2log on environments such as OSX/Clang, Ubuntu, CentOS, Windows/mingw, Windows/Visual Studio.  The focus on g2log is stability and compiler support. Only well, time tested, features from g3log will make it into g2log. 




# G3log with sinks
[Sinks](http://en.wikipedia.org/wiki/Sink_(computing)) are receivers of LOG calls. G3log comes with a default sink (*the same as G2log uses*) that can be used to save log to file.  A sink can be of *any* class type without restrictions as long as it can either receive a LOG message as a  *std::string* **or** as a *g2::LogMessageMover*. 

The *std::string* comes pre-formatted. The *g2::LogMessageMover* is a wrapped struct that contains the raw data for custom handling in your own sink.

A sink is *owned* by the G3log and is added to the logger inside a ```std::unique_ptr```.  The sink can be called though its public API through a *handler* which will asynchronously forward the call to the receiving sink. 
```
auto sinkHandle = logworker->addSink(std2::make_unique<CustomSink>(),
                                     &CustomSink::ReceiveLogMessage);
```

#Code Examples
Example usage where a custom sink is added. A function is called though the sink handler to the actual sink object.
```
// main.cpp
#include<g2log.hpp>
#include<g2logworker.hpp>
#include <std2_make_unique.hpp>

#include "CustomSink.h"

int main(int argc, char**argv) {
   using namespace g2;
   std::unique_ptr<LogWorker> logworker{ LogWorker::createWithNoSink() };
   auto sinkHandle = logworker->addSink(std2::make_unique<CustomSink>(),
                                          &CustomSink::ReceiveLogMessage);
   
   // initialize the logger before it can receive LOG calls
   initializeLogging(logworker.get());
   LOG(WARNING) << "This log call, may or may not happend before"
                << "the sinkHandle->call below";
				
				
   // You can call in a thread safe manner public functions on your sink
   // The call is asynchronously executed on your custom sink.
   std::future<void> received = sinkHandle->call(&CustomSink::Foo, 
                                                 param1, param2);
   
   // If the LogWorker is initialized then at scope exit the g2::shutDownLogging() will be called. 
   // This is important since it protects from LOG calls from static or other entities that will go out of
   // scope at a later time. 
   //
   // It can also be called manually:
   g2::shutDownLogging();
}


// some_file.cpp : To show how easy it is to get the logger to work
// in other parts of your software

#include <g2log.hpp>

void SomeFunction() {
   ...
   LOG(INFO) << "Hello World";
}
```

Example usage where a the default file logger is used **and** a custom sink is added
```
// main.cpp
#include<g2log.hpp>
#include<g2logworker.hpp>
#include <std2_make_unique.hpp>

#include "CustomSink.h"

int main(int argc, char**argv) {
   using namespace g2;
   auto defaultHandler = LogWorker::createWithDefaultLogger(argv[0], 
                                                 path_to_log_file);
   
   // logger is initialized
   g2::initializeLogging(defaultHandler.worker.get());
   
   LOG(DEBUG) << "Make log call, then add another sink";
   
   defaultHandler.worker->addSink(std2::make_unique<CustomSink>(),
                                  &CustomSink::ReceiveLogMessage);
   
   ...
}
```



# BUILDING g3log: 
-----------
The default is to build an example binary 'g3log-FATAL-contract' and 'g3log-FATAL-sigsegv'. I suggest you start with that, run it and view the created log also.

If you are interested in the performance or unit tests then you can 
enable the creation of them in the g3log/CMakeLists.txt file. See that file for 
more details


```
cd g3log
mkdir build
cd build
```

** Building on Linux **
```
cmake -DCMAKE_BUILD_TYPE=Release ..
make 
```

** Building on Windows **
Please use the Visual Studio 12 (2013) command prompt "Developer command prompt"
```
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 12" ..
msbuild g3log.sln /p:Configuration=Release
```

** Building on *nix with Clang:  **
```
cmake -DCMAKE_CXX_COMPILER=clang++      -DCMAKE_BUILD_TYPE=Release ..
make 
```

#Performance
G3log aims to keep all background logging to sinks with as little log overhead as possible to the logging sink and with as small "worst case latency" as possible. For this reason g3log is a good logger for many systems that deal with critical tasks. Depending on platform the average logging overhead will differ. On my laptop the average call, when doing extreme performance testing, will be about ~2 us.

The worst case latency is kept stabile with no extreme peaks, in spite of any sudden extreme pressure.  I have a blog post regarding comparing worst case latency for g3log and other loggers which might be of interest. 
You can find it here: https://kjellkod.wordpress.com/2015/06/30/the-worlds-fastest-logger-vs-g3log/
     
#Enjoy
If you like this logger (or not) it would be nice with some feedback. That way I can improve g3log and g2log and it is also nice to see if someone is using it.

 If you have ANY questions or problems please do not hesitate in contacting me on my blog 
http://kjellkod.wordpress.com/2011/11/17/kjellkods-g2log-vs-googles-glog-are-asynchronous-loggers-taking-over  
or at ```Hedstrom at KjellKod dot cc```


Cheers

Kjell *(a.k.a. KjellKod)*
