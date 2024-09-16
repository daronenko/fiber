cmake_minimum_required(VERSION 3.20)
project(fiber)

# CMake options

include(cmake/Logging.cmake)
include(cmake/Ccache.cmake)
include(cmake/CompileOptions.cmake)

# Third party CMakeLists
add_subdirectory(third_party)

# This must be included after library so that
# third party code is not affected
include(cmake/Werror.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(cmake/Sanitize.cmake)
endif()

set(LIB_TARGET ${PROJECT_NAME})

# CMakeLists
add_subdirectory(source)
add_subdirectory(playground)
add_subdirectory(tests)
