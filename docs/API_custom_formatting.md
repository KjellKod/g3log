[introduction](index.md) | [detailed information](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [**Custom log formatting**](API_custom_formatting.md)


# Custom LOG <a name="log_formatting">formatting</a>
### Overriding the Default File Sink's file header
The default file header can be customized in the default file sink in calling 
```cpp
   FileSink::overrideLogHeader(std::string);
```


### Overriding the Default FileSink's log formatting
The default log formatting is defined in `LogMessage.hpp`
```cpp
   static std::string DefaultLogDetailsToString(const LogMessage& msg);
```

### Adding thread ID to the log formatting
An "all details" log formatting function is also defined - this one also adds the "calling thread's ID"
```cpp
   static std::string FullLogDetailsToString(const LogMessage& msg);
```

### Override default sink log formatting
For convenience the *Default* sink has a function
for doing exactly this
```cpp
  void overrideLogDetails(LogMessage::LogDetailsFunc func);
```


Example code for replacing the default log formatting for "full details" formatting (it adds thread ID)

```cpp
   auto worker = g3::LogWorker::createLogWorker();
   auto handle= worker->addDefaultLogger(argv[0], path_to_log_file);
   g3::initializeLogging(worker.get());
   handle->call(&g3::FileSink::overrideLogDetails, &LogMessage::FullLogDetailsToString);
```

See [test_message.cpp](https://github.com/KjellKod/g3log/tree/master/test_unit/test_message.cpp) for details and testing


Example code for overloading the formatting of a custom sink. The log formatting function will be passed into the 
`LogMessage::toString(...)` this will override the default log formatting

Example
```cpp
namespace {
      std::string MyCustomFormatting(const LogMessage& msg) {
        ... how you want it ...
      }
    }

   void MyCustomSink::ReceiveLogEntry(LogMessageMover message) {
      std::string formatted = message.get().toString(&MyCustomFormatting) << std::flush;
   }
...
...
 auto worker = g3::LogWorker::createLogWorker();
 auto sinkHandle = worker->addSink(std::make_unique<MyCustomSink>(),
                                     &MyCustomSink::ReceiveLogMessage);
 // ReceiveLogMessage(...) will used the custom formatting function "MyCustomFormatting(...)
    
```


[introduction](index.md) | [detailed information](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [**Custom log formatting**](API_custom_formatting.md)

