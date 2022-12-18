# - Find GNU gettext tools
# This module looks for the GNU gettext tools. This module defines the 
# following values:
#  GETTEXT_MSGMERGE_EXECUTABLE: the full path to the msgmerge tool.
#  GETTEXT_MSGFMT_EXECUTABLE: the full path to the msgfmt tool.
#  GETTEXT_FOUND: True if gettext has been found.
#
# Additionally it provides the following macros:
# GETTEXT_CREATE_krb5tw_translations ( outputFile [ALL] file1 ... fileN )
#    This will create a target "krb5tw_translations" which will convert the 
#    given input po files into the binary output mo file. If the 
#    ALL option is used, the krb5tw_translations will also be created when
#    building the default target.


find_program(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)

find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

macro(gettext_create_krb5tw_translations _potFile _allOption)
  set(_gmoFiles)
  get_filename_component(_potBasename ${_potFile} NAME_WE)
  get_filename_component(_absPotFile ${_potFile} ABSOLUTE)

  set(_addToAll)
  set(_firstPoFile)
  if (${_allOption} STREQUAL "ALL")
    set(_addToAll "ALL")
  else (${_allOption} STREQUAL "ALL")
    set(_firstPoFile ${_allOption})
  endif (${_allOption} STREQUAL "ALL")

  foreach (_currentPoFile ${_firstPoFile} ${ARGN})

    #MESSAGE( STATUS "${_currentPoFile}" )

    get_filename_component(_absFile ${_currentPoFile} ABSOLUTE)
    get_filename_component(_abs_PATH ${_absFile} PATH)
    get_filename_component(_lang ${_absFile} NAME_WE)

    #MESSAGE( STATUS "LANG: ${_lang}")

    set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

    #MESSAGE( STATUS "GMO: ${_gmoFile}" )

    add_custom_command(
        OUTPUT ${_gmoFile}
        COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
        DEPENDS ${_absPotFile} ${_absFile}
    )

    install(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo COMPONENT localization)
    set(_gmoFiles ${_gmoFiles} ${_gmoFile})

  endforeach (_currentPoFile)

  add_custom_target(krb5tw_translations ${_addToAll} DEPENDS ${_gmoFiles})

endmacro(gettext_create_krb5tw_translations)

if (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE)
  set(GETTEXT_FOUND TRUE)
else (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE)
  set(GETTEXT_FOUND FALSE)
  if (GetText_REQUIRED)
    message(FATAL_ERROR "GetText not found")
  endif (GetText_REQUIRED)
endif (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE)



