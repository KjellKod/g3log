#.rst:
# FindG3log
# -------
#
# Find libg3logger, G3log is an asynchronous, "crash safe", logger that is easy to use with default logging sinks or you can add your own.
#
# This defines the cmake import target "g3logger" you can use like this
#```
# target_link_libraries(YourTarget PUBLIC g3logger)
#```
# Variables and features 
# ----------------------
# * ``G3LOG`` -- if this environment variable is set, it'll be used as a hint as to where the g3logger files are. 
# * ``G3LOG_INCLUDE_DIRS`` -- raw cmake variable with include path 
# * ``G3LOG_LIBRARIES`` -- raw cmake variable with library link line
# * ``G3LOG_FOUND`` -- check if the lib was found without using the newer ``if(TARGET g3logger)...``

include(FindPackageHandleStandardArgs)
find_path(G3LOG_INCLUDE_DIR 
    NAMES
         g3log/active.hpp
         g3log/atomicbool.hpp
         g3log/crashhandler.hpp
         g3log/filesink.hpp
         g3log/future.hpp
         g3log/g3log.hpp
         g3log/generated_definitions.hpp
         g3log/logcapture.hpp
         g3log/loglevels.hpp
         g3log/logmessage.hpp
         g3log/logworker.hpp
         g3log/moveoncopy.hpp
         g3log/shared_queue.hpp
         g3log/sinkhandle.hpp
         g3log/sink.hpp
         g3log/sinkwrapper.hpp
         g3log/stacktrace_windows.hpp
         g3log/stlpatch_future.hpp
         g3log/time.hpp
    PATHS        
        ENV G3LOG
)
find_library(G3LOG_LIBRARY
            NAMES libg3logger g3logger )
find_package_handle_standard_args(g3logger
            REQUIRED_VARS G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
mark_as_advanced(G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
set(G3LOG_LIBRARIES ${G3LOG_LIBRARY})
set(G3LOG_INCLUDE_DIRS ${G3LOG_INCLUDE_DIR})

add_library(g3logger SHARED IMPORTED)
find_package(Threads REQUIRED)
set_target_properties(g3logger PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${G3LOG_INCLUDE_DIRS}"
    IMPORTED_LOCATION "${G3LOG_LIBRARY}"
    IMPORTED_LINK_INTERFACE_LIBRARIES Threads::Threads
    )
