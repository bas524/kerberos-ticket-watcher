find_package(Git)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%H
        OUTPUT_VARIABLE COMMITTER_FULLSHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%h
        OUTPUT_VARIABLE COMMITTER_SHORTSHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%ci
        OUTPUT_VARIABLE COMMITTER_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)