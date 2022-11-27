find_package(Git)

if (NOT DEFINED COMMITTER_FULLSHA)
  execute_process(
          COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%H
          OUTPUT_VARIABLE COMMITTER_FULLSHA
          OUTPUT_STRIP_TRAILING_WHITESPACE
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif ()

if (NOT DEFINED COMMITTER_SHORTSHA)
  execute_process(
          COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%h
          OUTPUT_VARIABLE COMMITTER_SHORTSHA
          OUTPUT_STRIP_TRAILING_WHITESPACE
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif ()

if (NOT DEFINED COMMITTER_DATE)
  execute_process(
          COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%ci
          OUTPUT_VARIABLE COMMITTER_DATE
          OUTPUT_STRIP_TRAILING_WHITESPACE
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif ()
configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)