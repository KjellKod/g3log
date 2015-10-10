# API description
Most of the API that you need for using g3log is described in this readme. For more API documentation and examples please continue to read the [API readme](API.markdown). Examples of what you will find here are: 

* Logging API: LOG calls
* Contract API: CHECK calls
* Logging levels 
  * disable/enabled levels at runtime
  * custom logging levels
* Sink [creation](#sink_creation) and utilization 
* Fatal handling
  * custom fatal handling
  * pre fatal hook
  * override of signal handling
  * disable fatal handling
* LOG calls
* CHECK calls

## Logging API: LOG calls
It is optional to use either streaming ```LOG(INFO) << some text ``` or printf-like syntax ```LOGF(WARNING, "some number %d", 123); ```

Conditional logging is made with ```LOG_IF(INFO, <boolean-expression>) << " some text"  ``` or ```LOGF_IF(WARNING, <boolean-expression>) << " some text".```  Only if the expressions evaluates to ```true``` will the logging take place. 

Example:
```LOG_IF(INFO, 1 = 200) << " some text";```   or ```LOG_IF(FATAL, SomeFunctionCall()) << " some text";```

*<a name="fatal_logging">A call using FATAL</a>  logging level, such as the ```LOG_IF(FATAL,...)``` example above, will after logging the message at ```FATAL```level also kill the process.  It is essentially the same as a ```CHECK(<boolea-expression>) << ...``` with the difference that the ```CHECK(<boolean-expression)``` triggers when the expression evaluates to ```false```.*

## Contract API: CHECK calls
The contract API follows closely the logging API with ```CHECK(<boolean-expression>) << ...``` for streaming  or ```CHECK_F(<boolean-expression>, ...);``` for printf-style.

If the ```<boolean-expression>``` evaluates to false then the the message for the failed contract will be logged in FIFO order with previously made messages. The process will then shut down after the message is sent to the sinks and the sinks have dealt with the fatal contract message. 

## Logging levels ```
 The default logging levels are ```DEBUG```, ```INFO```, ```WARNING``` and ```FATAL``` (see FATAL usage [above](#fatal_logging))

  ### disable/enabled levels at runtime
  ### custom logging levels```FATAL``` (see FATAL usage above)
## Sink <a name="sink_creation">creation</a> and utilization 
## Fatal handling
  ### custom fatal handling
  ### pre fatal hook
  ### override of signal handlingboolean-expression>
  ### disable fatal handling


