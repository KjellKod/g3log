[**introduction**](index.md) | [detailed information](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)


# Welcome to g3log

G3log is an asynchronous logger with three main features: 
1. Intuitive `LOG(...)` API
2. `Design-by-contract` `CHECK(...)` functionality
3. Fatal crash handling for graceful shutdown of the logged process without loosing any log details up to the point of the crash.

The super quick introduction to g3log can be seen in the steps 1 - 9 below. 

For more in-depth information please see the full usage description in [g3log.md](g3log.md). The internal API for more advanced integration with g3log can be accessed in [API.md](API.md)

## 1. Easy usage in files
Avoid deep dependency injection complexity and instead get access to the logger as easy as 
```
#include <g3log/g3log.hpp>
``` 


## 2. Access to streaming and print_f log call syntax
Both streaming syntax `LOG` and print_f `LOGF` syntax are available. 

```
LOGF(INFO, "Hi log %d", 123);
LOG(INF) << "Hi log " << 123;

```

## 3. Conditional logging

```
LOG_IF(INFO, (1 < 2)) << "If true this message will be logged";
LOGF_IF(INFO, (1 < 2), "If true, then this %s will be logged", "message");
```

## 4. Design-by-contract framework 
```
CHECK(less != more); // not fatal
CHECK_F(less > more, "CHECK(false) will trigger a fatal message")
```

## 5. Handling of fatal 
By default g3log will capture fatal events such as `LOG(FATAL)`, `CHECK(false)` and otherwise fatal signals such as: 
```
    SIGABRT
    SIGFPE
    SIGILL
    SIGSEGV
    SIGTERM
``` 

When a fatal event happens the not-yet written log activity will be flushed to the logging sinks. Only when all logging activity up to the point of the fatal event has happend, will g3log allow the fatal event to proceed and exit the process. 

If `object` symbols are available the fatal handler will attempt to push the stacktrace up to the fatal reason to the logging sink. 

#### 5b. Overriding and customization of fatal event handling
For overriding fatal error handling to use your own, or to add code `hooks` that you want to execute please see the [API.md](API.md) doc. 

## 6. Default and Custom logging levels
The default logging levels are `DEBUG`, `INFO`, `WARNING` and `FATAL`. You can define your own logging levels or completely replace the logging levels. Ref: [API.md](API.md)


### 7. Log filtering
Log filtering is handled in g3log if dynamic logging levels are enabled
in the configuration. See the [API.md](API.md) for information. Log filtering can also be handled through the sink as can be seen in [github/Kjellod/g3sinks](https://github.com/KjellKod/g3sinks)


## 8. 3rd party and custom logging sinks
The default logging sink has no external 3rd party dependencies. For more logging sinks please see [github/Kjellod/g3sinks](https://github.com/KjellKod/g3sinks)

- log rotate
- log to syslog
- log to colored terminal output
- log rotate with filter

See the [API.md](API.md) for more information about the simple steps to creating your own logging sink.


## 9. Log instantiation 
With the default application name left as is (i.e. "g3log") a creation of the logger could look something like this: 

```cpp
  const std::string directory = "./";
  const std::string name = "TestLogFile";
  auto worker = g3::LogWorker::createLogWorker();
  auto handle = worker->addDefaultLogger(name, directory);
```
The resulting filename would be something like: 
```
   ./TestLogFile.g3log.20160217-001406.log
```

## <a name="performance">Performance</a>
G3log aims to keep all background logging to sinks with as little log overhead as possible to the logging sink and with as small "worst case latency" as possible. For this reason g3log is a good logger for many systems that deal with critical tasks. Depending on platform the average logging overhead will differ. On my 2010 laptop the average call, when doing extreme performance testing, will be about ~2 us.

The worst case latency is kept stable with no extreme peaks, in spite of any sudden extreme pressure.  I have a blog post regarding comparing worst case latency for g3log and other loggers which might be of interest.
You can find it here: https://kjellkod.wordpress.com/2015/06/30/the-worlds-fastest-logger-vs-g3log/


## <a name="continuos_integration">Continuos Integration</a>
The g3log repository is evaluating both github actions and CircleCI for executing test coverage, installation and document generation. For windows the repo is still relying on appveyor.  In case you want to look into change any of these setups the following files are the ones of interest. 
```
1. appveyor --> g3log/appveyor.yml
2. circleCI --> g3log/.circleci/config.yml
3. github actions --> g3log/.github/workflows/*.yml


```


## <a name="feedback">Feedback</a>
If you like this logger (or not) it would be nice with some feedback. That way I can improve g3log and it is always nice to hear when and how someone is using it. 

 If you have ANY questions or problems please do not hesitate in contacting me at 
 `Hedstrom @ Kjellod. cc`

# <a name="say-thanks">Say Thanks</a>
This logger is available for free and all of its source code is public domain.  A great way of saying thanks is to send a donation. It would go a long way not only to show your support but also to boost continued development.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/g3log/25)

* $5 for a cup of coffee
* $25 for a late evening coding with takeout 


Cheers

Kjell *(a.k.a. KjellKod)*

[**introduction**](index.md) | [detailed information](g3log.md) | [Configure & Build](building.md) | [API description](API.md) | [Custom log formatting](API_custom_formatting.md)
