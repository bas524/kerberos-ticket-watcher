# Copied from
# https://raw.githubusercontent.com/nfs-ganesha/nfs-ganesha/master/src/cmake/modules/FindKrb5.cmake
# Removed gssapi if not requested in find package invocation.
# Added version detection by Majlen
# - Find kerberos 5
# Find the native Kerberos 5 headers and libraries.
#  KRB5_INCLUDE_DIRS      - where to find krb5.h, etc.
#  KRB5_LIBRARIES         - List of libraries when using kerberos 5.
#  KRB5_FOUND             - True if kerberos 5 found.
#  KRB5_VERSION           - Kerberos5 libraries version
# KRB5 modules may be specified as components for this find module.
# Modules may be listed by running "krb5-config".  Modules include:
#  krb5              Kerberos 5 application
#  gssapi            GSSAPI application with Kerberos 5 bindings
#  krb4              Kerberos 4 application
#  kadm-client       Kadmin client
#  kadm-server       Kadmin server
#  kdb               Application that accesses the kerberos database
# Typical usage:
#  FIND_PACKAGE(KRB5 REQUIRED gssapi)

# First find the config script from which to obtain other values.
if (KRB5_PREFIX)
  find_program(KRB5_C_CONFIG NAMES krb5-config
               PATHS ${KRB5_PREFIX}
               NO_SYSTEM_ENVIRONMENT_PATH
               NO_DEFAULT_PATH
               )
endif (KRB5_PREFIX)
find_program(KRB5_C_CONFIG NAMES krb5-config PATHS /usr/lib/mit/bin)

message(STATUS "found krb5-config here ${KRB5_C_CONFIG}")

# Check whether we found anything.
if (KRB5_C_CONFIG)
  set(KRB5_FOUND 1)
else (KRB5_C_CONFIG)
  set(KRB5_FOUND 0)
endif (KRB5_C_CONFIG)

# Lookup the include directories needed for the components requested.
if (KRB5_FOUND)
  # Use the newer EXECUTE_PROCESS command if it is available.
  if (COMMAND EXECUTE_PROCESS)
    execute_process(
            COMMAND ${KRB5_C_CONFIG} ${KRB5_FIND_COMPONENTS} --cflags
            OUTPUT_VARIABLE KRB5_C_CONFIG_CFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE KRB5_C_CONFIG_RESULT
    )
  else (COMMAND EXECUTE_PROCESS)
    exec_program(${KRB5_C_CONFIG} ARGS "${KRB5_FIND_COMPONENTS} --cflags"
                 OUTPUT_VARIABLE KRB5_C_CONFIG_CFLAGS
                 RETURN_VALUE KRB5_C_CONFIG_RESULT
                 )
  endif (COMMAND EXECUTE_PROCESS)

  # Parse the include flags.
  if ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    # Convert the compile flags to a CMake list.
    string(REGEX REPLACE " +" ";"
           KRB5_C_CONFIG_CFLAGS "${KRB5_C_CONFIG_CFLAGS}")

    # Look for -I options.
    set(KRB5_INCLUDE_DIRS)
    foreach (flag ${KRB5_C_CONFIG_CFLAGS})
      if ("${flag}" MATCHES "^-I")
        string(REGEX REPLACE "^-I" "" DIR "${flag}")
        file(TO_CMAKE_PATH "${DIR}" DIR)
        set(KRB5_INCLUDE_DIRS ${KRB5_INCLUDE_DIRS} "${DIR}")
      endif ("${flag}" MATCHES "^-I")
    endforeach (flag)
  else ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    message("Error running ${KRB5_C_CONFIG}: [${KRB5_C_CONFIG_RESULT}]")
    set(KRB5_FOUND 0)
  endif ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
endif (KRB5_FOUND)

if (KRB5_PREFIX AND EXISTS "${KRB5_PREFIX}/include")
  set(KRB5_INCLUDE_DIRS "${KRB5_PREFIX}/include" ${KRB5_INCLUDE_DIRS})
endif (KRB5_PREFIX AND EXISTS "${KRB5_PREFIX}/include")

# Lookup the libraries needed for the components requested.
if (KRB5_FOUND)
  # Use the newer EXECUTE_PROCESS command if it is available.
  if (COMMAND EXECUTE_PROCESS)
    execute_process(
            COMMAND ${KRB5_C_CONFIG} ${KRB5_FIND_COMPONENTS} --libs
            OUTPUT_VARIABLE KRB5_C_CONFIG_LIBS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE KRB5_C_CONFIG_RESULT
    )
  else (COMMAND EXECUTE_PROCESS)
    exec_program(${KRB5_C_CONFIG} ARGS "${KRB5_FIND_COMPONENTS} --libs"
                 OUTPUT_VARIABLE KRB5_C_CONFIG_LIBS
                 RETURN_VALUE KRB5_C_CONFIG_RESULT
                 )
  endif (COMMAND EXECUTE_PROCESS)

  # Parse the library names and directories.
  if ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    string(REGEX REPLACE " +" ";"
           KRB5_C_CONFIG_LIBS "${KRB5_C_CONFIG_LIBS}")

    # Look for -L flags for directories and -l flags for library names.
    set(KRB5_LIBRARY_DIRS)
    set(KRB5_LIBRARY_NAMES)
    foreach (flag ${KRB5_C_CONFIG_LIBS})
      if ("${flag}" MATCHES "^-L")
        string(REGEX REPLACE "^-L" "" DIR "${flag}")
        file(TO_CMAKE_PATH "${DIR}" DIR)
        set(KRB5_LIBRARY_DIRS ${KRB5_LIBRARY_DIRS} "${DIR}")
      elseif ("${flag}" MATCHES "^-l")
        string(REGEX REPLACE "^-l" "" NAME "${flag}")
        set(KRB5_LIBRARY_NAMES ${KRB5_LIBRARY_NAMES} "${NAME}")
      endif ("${flag}" MATCHES "^-L")
    endforeach (flag)

    # add gssapi_krb5 (MIT)
    # SET(KRB5_LIBRARY_NAMES ${KRB5_LIBRARY_NAMES} "gssapi_krb5")

    # Search for each library needed using the directories given.
    foreach (name ${KRB5_LIBRARY_NAMES})
      # Look for this library.
      find_library(KRB5_${name}_LIBRARY
                   NAMES ${name}
                   PATHS ${KRB5_LIBRARY_DIRS}
                   NO_DEFAULT_PATH
                   )
      find_library(KRB5_${name}_LIBRARY NAMES ${name})
      mark_as_advanced(KRB5_${name}_LIBRARY)

      # If any library is not found then the whole package is not found.
      if (NOT KRB5_${name}_LIBRARY)
        set(KRB5_FOUND 0)
      endif (NOT KRB5_${name}_LIBRARY)

      # Build an ordered list of all the libraries needed.
      set(KRB5_LIBRARIES ${KRB5_LIBRARIES} "${KRB5_${name}_LIBRARY}")
    endforeach (name)
  else ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    message("Error running ${KRB5_C_CONFIG}: [${KRB5_C_CONFIG_RESULT}]")
    set(KRB5_FOUND 0)
  endif ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
endif (KRB5_FOUND)

# Lookup the version
if (KRB5_FOUND)
  # Use the newer EXECUTE_PROCESS command if it is available.
  if (COMMAND EXECUTE_PROCESS)
    execute_process(
            COMMAND ${KRB5_C_CONFIG} --version
            OUTPUT_VARIABLE KRB5_C_CONFIG_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE KRB5_C_CONFIG_RESULT
    )
  else (COMMAND EXECUTE_PROCESS)
    exec_program(${KRB5_C_CONFIG} ARGS "--version"
                 OUTPUT_VARIABLE KRB5_C_CONFIG_VERSION
                 RETURN_VALUE KRB5_C_CONFIG_RESULT
                 )
  endif (COMMAND EXECUTE_PROCESS)

  # Parse the include flags.
  if ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    # Convert the compile flags to a CMake list.
    string(REGEX REPLACE "^Kerberos 5 release " ""
           KRB5_VERSION "${KRB5_C_CONFIG_VERSION}")
  else ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
    message("Error running ${KRB5_C_CONFIG}: [${KRB5_C_CONFIG_RESULT}]")
    set(KRB5_FOUND 0)
  endif ("${KRB5_C_CONFIG_RESULT}" MATCHES "^0$")
endif (KRB5_FOUND)

# Report the results.
if (NOT KRB5_FOUND)
  set(KRB5_DIR_MESSAGE
      "KRB5 was not found. Make sure the entries KRB5_* are set.")
  if (NOT KRB5_FIND_QUIETLY)
    message(STATUS "${KRB5_DIR_MESSAGE}")
  else (NOT KRB5_FIND_QUIETLY)
    if (KRB5_FIND_REQUIRED)
      message(FATAL_ERROR "${KRB5_DIR_MESSAGE}")
    endif (KRB5_FIND_REQUIRED)
  endif (NOT KRB5_FIND_QUIETLY)
else (NOT KRB5_FOUND)
  set(CMAKE_REQUIRED_INCLUDES ${KRB5_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${KRB5_LIBRARIES})
  if (NOT DEFINED KRB5_USE_KRB5H)
    include(CheckIncludeFile)
    check_include_file("krb5/krb5.h" KRB5_krb5_krb5_h_exists)
    if (NOT KRB5_krb5_krb5_h_exists)
      check_include_file("krb5.h" KRB5_USE_KRB5H)
    else (NOT KRB5_krb5_krb5_h_exists)
      set(KRB5_USE_KRB5H FALSE)
    endif (NOT KRB5_krb5_krb5_h_exists)
  endif (NOT DEFINED KRB5_USE_KRB5H)
  include(CheckFunctionExists)
  if (NOT DEFINED KRB5_FLAVOR)
    check_function_exists(krb5_free_keytab_entry_contents
                          KRB5_HAS_krb5_free_keytab_entry_contents)
    if (KRB5_HAS_krb5_free_keytab_entry_contents)
      set(KRB5_FLAVOR "MIT")
    else (KRB5_HAS_krb5_free_keytab_entry_contents)
      set(KRB5_FLAVOR "HEIMDAL")
    endif (KRB5_HAS_krb5_free_keytab_entry_contents)
  endif (NOT DEFINED KRB5_FLAVOR)
  if ("${KRB5_FLAVOR}" STREQUAL "MIT")
    if (NOT DEFINED KRB5_HAS_krb5_get_init_creds_opt_set_out_ccache)
      check_function_exists(krb5_get_init_creds_opt_set_out_ccache
                            KRB5_HAS_krb5_get_init_creds_opt_set_out_ccache)
    endif (NOT DEFINED KRB5_HAS_krb5_get_init_creds_opt_set_out_ccache)
    if (NOT DEFINED KRB5_HAS_krb5_unparse_name_flags_ext)
      check_function_exists(krb5_unparse_name_flags_ext
                            KRB5_HAS_krb5_unparse_name_flags_ext)
    endif (NOT DEFINED KRB5_HAS_krb5_unparse_name_flags_ext)
  endif ("${KRB5_FLAVOR}" STREQUAL "MIT")

  message(STATUS "Found kerberos 5 headers:    ${KRB5_INCLUDE_DIRS}")
  message(STATUS "Found kerberos 5 libs:       ${KRB5_LIBRARIES}")
  message(STATUS "Detected kerberos 5 version: ${KRB5_VERSION}")
  message(STATUS "Detected kerberos 5 flavor:  ${KRB5_FLAVOR}")
endif (NOT KRB5_FOUND)