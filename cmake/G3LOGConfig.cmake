#.rst:
# FindG3log
# -------
#
# Find libg3logger, G3log is an asynchronous, "crash safe", logger that is easy to use with default logging sinks or you can add your own.
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``G3LOG_INCLUDE_DIRS``
#   where to find g3log.hpp, etc.
# ``G3LOG_LIBRARIES``
#   the libraries to link against to use libg3logger.
#   that includes libg3logger library files.
# ``G3LOG_FOUND``
#   If false, do not try to use G3LOG.
include(FindPackageHandleStandardArgs)

find_path(G3LOG_INCLUDE_DIR g3log/g3log.hpp)

find_library(G3LOG_LIBRARY
                NAMES libg3logger g3logger)

find_package_handle_standard_args(G3LOG  DEFAULT_MSG
                                    G3LOG_INCLUDE_DIR G3LOG_LIBRARY)

mark_as_advanced(G3LOG_INCLUDE_DIR G3LOG_LIBRARY )

set(G3LOG_LIBRARIES ${G3LOG_LIBRARY} )
set(G3LOG_INCLUDE_DIRS ${G3LOG_INCLUDE_DIR} )
