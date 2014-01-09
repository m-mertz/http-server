cmake_minimum_required (VERSION 2.6)
project (http-server)

function (find_cc_sources ARGV RESULT_NAME)
  set (FOUND_SOURCES)
  foreach (DIR ${ARGV})
    file (GLOB_RECURSE TMP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.cc)
    list (APPEND FOUND_SOURCES ${TMP_SOURCES})
  endforeach (DIR)
  set (${RESULT_NAME} ${FOUND_SOURCES} PARENT_SCOPE)
endfunction ()

add_subdirectory (server)
