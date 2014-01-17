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
CHECK(less > more) << "CHECK(false) triggers a FATAL message");
```


## What G3Log is: 
* ***G3log*** is the acting name for the third version of g2log and it stands for **g2log with dynamic sinks**
* G3log is an asynchronous, "crash-safe" logger. You can read more about it here [[g2log version]](
http://www.codeproject.com/Articles/288827/g2log-An-efficient-asynchronous-logger-using-Cplus)


## Benefits you get when using G3log ##
1. Easy to use, clean syntax and blazingly fast logger. 

2. All the slow log I/O disk access is done in a background thread. This ensures that the LOG caller can immediately continue with other tasks and do not have to wait for the LOG call to fínish.

3. G3log provides logging, Design-by-Contract [#CHECK], and flush of log to file at
 shutdown. Buffered logs will be written to the sink before the application shuts down.

4. It is thread safe, so using it from multiple threads is completely fine. 

5. It is *CRASH SAFE*. It will save the made logs to the sink before it shuts down. 
The logger will catch certain fatal signals, so if your application  crashes due to, say a segmentation fault, *SIGSEGV*,  or some other fatal signal it will  log and save the crash and all previously buffered log
 entries before exiting.

 
6. It is cross platform. Tested and used by me or by clients on OSX, Windows, Ubuntu, CentOS

7. On *Nix* systems a caught fatal signal will generate a stack dump to the log. A Beta version exist on Windows and can be released on request.
 

8. G2log is used world wide in commercial products as well as hobby projects since early 2011.
The code is given for free as public domain. This gives the option to change, use,
 and do whatever with it, no strings attached.

9. Three versions of g2log exist. 
    * This version: *g3log* : which is made to faciliate easy adding of custom log receivers. 
    * [g2log-dev](https://bitbucket.org/KjellKod/g2log-dev): Acting as feature try-out and playground. 
    * *[g2log](https://bitbucket.org/KjellKod/g2log)*: The original. Simple, easy to modify and with the most OS support. Clients use g2log on environments such as OSX/Clang, Ubuntu, CentOS, Windows/mingw, Windows/Visual Studio. 





# G3log with sinks
[Sinks](http://en.wikipedia.org/wiki/Sink_(computing)) are receivers of LOG calls. G3log comes with a default sink (the same as G2log uses) that can be used to save log to file.  A sink can be of *any* class type without restrictions as long as it can either recive a LOG message as a  **std::string** *or* a **g2::LogMessageMover**. The **std::string** comes pre-formatted while the **g2::LogMessageMover** contains the raw data for custom handling in your own sink.




# BUILDING g2log: 
-----------
The default is to build an example binary 'g2log-FATAL'
I suggest you start with that, run it and view the created log also.

If you are interested in the performance or unit tests then you can 
enable the creation of them in the g2log/CMakeLists.txt file. See that file for 
more details

*** Building on Linux ***
cd g2log
mkdir build
cd build 
cmake ..
make

*** Building on Windows ***
Please use the Visual Studio 11 (2012) command prompt "Developer command prompt"
cd g2log
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 11" ..
msbuild g2log_by_kjellkod.sln /p:Configuration=Release
Release\g2log-FATAL.exe


      
#SOURCE CONTENTS
3rdParty -- gtest, glog. 
-----------------------
*gtest is needed for the unit tests. 
*glog is only needed if you want to run the glog vs g2log comparison tests



If you like this logger (or not) it would be nice with some feedback. That way I can 
improve g2log and it is also nice to see if someone is using it. If you have 
ANY questions or problems please do not hesitate in contacting me on my blog 
http://kjellkod.wordpress.com/2011/11/17/kjellkods-g2log-vs-googles-glog-are-asynchronous-loggers-taking-over/
or at <Hedstrom at KjellKod dot cc>

Good luck :)

Cheers
Kjell (a.k.a. KjellKod)
