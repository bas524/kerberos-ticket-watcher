find_package(Git)

execute_process(
        COMMAND "${GIT_EXECUTABLE} rev-list HEAD --count"
        OUTPUT_VARIABLE BUILD_VER
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%H
        OUTPUT_VARIABLE COMMITTER_FULLSHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%h
        OUTPUT_VARIABLE COMMITTER_SHORTSHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%ci
        OUTPUT_VARIABLE COMMITTER_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_custom_target(TARGET VersionHeader
                  PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/modules/version.cmake
                  -DSRC=${CMAKE_SOURCE_DIR}/src/version.h.in
                  -DDST=${CMAKE_BINARY_DIR}/version.h
                  -DBUILD_VER="${BUILD_VER}"
                  -DCOMMITTER_FULLSHA="${COMMITTER_FULLSHA}"
                  -DCOMMITTER_SHORTSHA="${COMMITTER_SHORTSHA}"
                  -DCOMMITTER_DATE="${COMMITTER_DATE}"
                  )

set_target_properties(VersionHeader
                      PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES
                      ${CMAKE_BINARY_DIR})

