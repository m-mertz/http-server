cmake_minimum_required(VERSION 2.6)
project(http-server)

# Enable debug build.
set(CMAKE_BUILD_TYPE Debug)

# Enable compiler warnings with -Wall if gcc is used as compiler.
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wshadow -Weffc++")
endif(CMAKE_COMPILER_IS_GNUCC)

# Function for finding all .cc files in a directory recursively and
# storing their paths in a variable.
function(find_cc_sources ARGV RESULT_NAME)
  set(FOUND_SOURCES)
  foreach(DIR ${ARGV})
    file(GLOB_RECURSE TMP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.cc)
    list(APPEND FOUND_SOURCES ${TMP_SOURCES})
  endforeach(DIR)
  set(${RESULT_NAME} ${FOUND_SOURCES} PARENT_SCOPE)
endfunction()

# Add this folder as include dir, so that all includes of project headers can be
# relative to the project source root.
include_directories(.)

# Process subdirectories containing the components.
add_subdirectory(util)
add_subdirectory(server)
add_subdirectory(client)
