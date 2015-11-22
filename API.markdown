# API description
Most of the API that you need for using g3log is described in this readme. For more API documentation and examples please continue to read the [API readme](API.markdown). Examples of what you will find here are: 

* Logging API: LOG calls
* Contract API: CHECK calls
* Logging levels 
  * disable/enabled levels at runtime
  * custom logging levels
* Sink [creation](#sink_creation) and utilization 
* LOG [flushing](#log_flushing)
* Fatal handling
  * custom fatal handling
  * pre fatal hook
  * override of signal handling
  * disable fatal handling
* Build Options


## Logging API: LOG calls
It is optional to use either streaming ```LOG(INFO) << some text ``` or printf-like syntax ```LOGF(WARNING, "some number %d", 123); ```

Conditional logging is made with ```LOG_IF(INFO, <boolean-expression>) << " some text"  ``` or ```LOGF_IF(WARNING, <boolean-expression>) << " some text".```  Only if the expressions evaluates to ```true``` will the logging take place. 

Example:
```LOG_IF(INFO, 1 != 200) << " some text";```   or ```LOG_IF(FATAL, SomeFunctionCall()) << " some text";```

*<a name="fatal_logging">A call using FATAL</a>  logging level, such as the ```LOG_IF(FATAL,...)``` example above, will after logging the message at ```FATAL```level also kill the process.  It is essentially the same as a ```CHECK(<boolea-expression>) << ...``` with the difference that the ```CHECK(<boolean-expression)``` triggers when the expression evaluates to ```false```.*

## Contract API: CHECK calls
The contract API follows closely the logging API with ```CHECK(<boolean-expression>) << ...``` for streaming  or  (*) ```CHECKF(<boolean-expression>, ...);``` for printf-style.


If the ```<boolean-expression>``` evaluates to false then the the message for the failed contract will be logged in FIFO order with previously made messages. The process will then shut down after the message is sent to the sinks and the sinks have dealt with the fatal contract message. 


(\* * ```CHECK_F(<boolean-expression>, ...);``` was the the previous API for printf-like CHECK. It is still kept for backwards compatability but is exactly the same as ```CHECKF``` *)


## Logging levels ```
 The default logging levels are ```DEBUG```, ```INFO```, ```WARNING``` and ```FATAL``` (see FATAL usage [above](#fatal_logging)). The logging levels are defined in [loglevels.hpp](src/g3log/loglevels.hpp).

 For some windows framework there is a clash with the ```DEBUG``` logging level. One of the CMake [Build options](#build_options) can be used to then change offending default level from ```DEBUG``` TO ```DBUG```.

 **CMake option: (default OFF) ** ```cmake -DCHANGE_G3LOG_DEBUG_TO_DBUG=ON ..``` 

  ### disable/enabled levels at runtime
  Logging levels can be disabled at runtime. The logic for this happens in
  [loglevels.hpp](src/g3log/loglevels.hpp), [loglevels.cpp](src/loglevels.cpp) and [g3log.hpp](src/g3log/g3log.hpp).

  There is a cmake option to enable the dynamic enable/disable of levels. 
  When the option is enabled there will be a slight runtime overhead for each ```LOG``` call when the enable/disable status is checked. For most intent and purposes this runtime overhead is negligable. 

  There is **no** runtime overhead for internally checking if a level is enabled//disabled if the cmake option is turned off. If the dynamic logging cmake option is turned off then all logging levels are enabled.

**CMake option: (default OFF)** ```cmake -DUSE_DYNAMIC_LOGGING_LEVELS=ON  ..``` 


  ### custom logging levels
  Custom logging levels can be created and used. When defining a custom logging level you set the value for it as well as the text for it. You can re-use values for other levels such as *INFO*, *WARNING* etc or have your own values.

   Any value with equal or higher value than the *FATAL* value will be considered a *FATAL* logging level. 

  Example:
  ```
  // In CustomLoggingLevels.hpp
  #include <g3log/loglevels.hpp>

  // all values with a + 1 higher than their closest equivalet
  // they could really have the same value as well.

  const LEVELS FYI {DEBUG.value + 1, {"For Your Information"}}; 
  const LEVELS CUSTOM {INFO.value + 1, {"CUSTOM"}}; 
  const LEVELS SEVERE {WARNING.value +1, {"SEVERE"}};
  const LEVELS DEADLY {FATAL.value + 1, {"DEADLY"}}; 
  ```



  
## Sink <a name="sink_creation">creation</a> and utilization 
The default sink for g3log is the one as used in g2log. It is a simple file sink with a limited API. The details for the default file sink can be found in [filesink.hpp](src/g3log/filesink.hpp), [filesink.cpp](src/filesink.cpp), [filesinkhelper.ipp](src/filesinkhelper.ipp)

More sinks can be found at [g3sinks](http://www.github.com/KjellKod/g3sinks) (log rotate, log rotate with filtering on levels)

A logging sink is not required to be a subclass of a specific type. The only requirement of a logging sink is that it can receive a logging message of 


### Using the default sink
Sink creation is defined in [logworker.hpp](src/g3log/logworker.hpp) and used in [logworker.cpp](src/logworker.cpp). For in-depth knowlege regarding sink implementation details you can look at [sinkhandle.hpp](src/g3log/sinkhandle.hpp) and [sinkwrapper.hpp](src/g3log/sinkwrapper.hpp)


## LOG <a name="log_flushing">flushing</a> 
The default file sink will flush each log entry as it comes in. For different flushing policies please take a look at g3sinks [logrotate and LogRotateWithFilters](http://www.github.com/KjellKod/g3sinks/logrotate).

At shutdown all enqueued logs will be flushed to the sink.  
At a discovered fatal event (SIGSEGV et.al) all enqueued logs will be flushed to the sink.

A programmatically triggered abrupt process exit such as a call to   ```exit(0)``` will of course not get the enqueued log entries flushed. Similary  a bug that does not trigger a fatal signal but a process exit will also not get the enqueued log entries flushed.  G3log can catch several fatal crashes and it deals well with RAII exits but magic is so far out of its' reach.



## Fatal handling
  ### custom fatal handling
  ### pre fatal hook
  ### override of signal handlingboolean-expression>
  ### disable fatal handling


  ## <a name="build_options">Build Options</a>
  The build options are defined in the file [Options.cmake](Options.cmake)

  build options are generated and saved to a header file. This avoid having to set the define options in the client source code


