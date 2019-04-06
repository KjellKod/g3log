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

@PACKAGE_INIT@

find_package(Threads REQUIRED)

if (NOT TARGET g3logger)
  include("${CMAKE_CURRENT_LIST_DIR}/g3loggerTargets.cmake")

  get_target_property(G3LOG_INCLUDE_DIR g3logger INTERFACE_INCLUDE_DIRECTORIES)

  get_target_property(G3LOG_LIBRARY_DEBUG g3logger IMPORTED_IMPLIB_DEBUG)
  if (G3LOG_LIBRARY_DEBUG MATCHES ".*-NOTFOUND")
    get_target_property(G3LOG_LIBRARY_DEBUG g3logger IMPORTED_LOCATION_DEBUG)
  endif ()

  get_target_property(G3LOG_LIBRARY_RELEASE g3logger IMPORTED_IMPLIB_RELEASE)
  if (G3LOG_LIBRARY_RELEASE MATCHES ".*-NOTFOUND")
    get_target_property(G3LOG_LIBRARY_RELEASE g3logger IMPORTED_LOCATION_RELEASE)
  endif ()

  if (NOT G3LOG_LIBRARY_DEBUG MATCHES ".*-NOTFOUND" AND
      NOT G3LOG_LIBRARY_RELEASE MATCHES ".*-NOTFOUND")
    set(G3LOG_LIBRARY
      debug ${G3LOG_LIBRARY_DEBUG}
      optimized ${G3LOG_LIBRARY_RELEASE}
    )
  elseif (NOT G3LOG_LIBRARY_DEBUG MATCHES ".*-NOTFOUND")
    set(G3LOG_LIBRARY ${G3LOG_LIBRARY_DEBUG})
  elseif (NOT G3LOG_LIBRARY_RELEASE MATCHES ".*-NOTFOUND")
    set(G3LOG_LIBRARY ${G3LOG_LIBRARY_RELEASE})  
  endif ()

  list(APPEND G3LOG_LIBRARY Threads::Threads)
  if (WIN32)
    list(APPEND G3LOG_LIBRARY DbgHelp.lib)
  endif ()
endif ()

find_package_handle_standard_args(G3LOG
            REQUIRED_VARS G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
mark_as_advanced(G3LOG_INCLUDE_DIR G3LOG_LIBRARY)
set(G3LOG_LIBRARIES ${G3LOG_LIBRARY})
set(G3LOG_INCLUDE_DIRS ${G3LOG_INCLUDE_DIR})

