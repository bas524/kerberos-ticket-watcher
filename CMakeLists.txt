# set project's name
project(krb5-ticket-watcher)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0075 NEW)

set(PACKAGE krb5-ticket-watcher)
set(VERSION 1.0.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(krb5-ticket-watcher)

set(KTW_SRCS
    src/kinitdialog.cpp
    src/kinitdialog.h
    src/krb5ticketwatcher.cpp
    src/krb5ticketwatcher.h
    src/main.cpp
    src/krb5exception.cpp
    src/krb5exception.h
    src/krb5context.cpp
    src/krb5context.h
    src/krb5creds.cpp
    src/krb5creds.h
    src/krb5fwd.h
    src/krb5ccache.cpp
    src/krb5ccache.h
    src/krb5principal.cpp
    src/krb5principal.h
    src/krb5cursor.cpp
    src/krb5cursor.h
    src/krb5ticket.cpp
    src/krb5ticket.h
    src/krb5credsopts.cpp
    src/krb5credsopts.h
    src/krb5timestamphelper.cpp
    src/krb5timestamphelper.h
    src/stacktrace.h
    )

# another list, this time it includes all header files that should be treated with moc
set(KTW_MOC_HDRS
    src/krb5ticketwatcher.h
    src/kinitdialog.h
    src/pwdialog.h
    src/pwchangedialog.h
    )

# some .ui files
set(KTW_UIS
    src/pwdialog.ui
    src/pwchangedialog.ui
    src/mainwidget.ui
    src/kinitdialog.ui
    )


# enable warnings
add_definitions(-Wall -DTEXTDOMAIN=krb5-ticket-watcher)

find_package(Qt5 COMPONENTS Core Widgets Gui Network REQUIRED)

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
  message(STATUS "Found Gettext: ${GETTEXT_SOURCE}")
  include_directories(${GETTEXT_INCLUDE_DIR})
else (GETTEXT_FOUND)
  message(FATAL_ERROR "Gettext not found")
endif (GETTEXT_FOUND)

# this will run uic on .ui files:
qt5_wrap_ui(KTW_UI_HDRS ${KTW_UIS} OPTIONS -tr ki18n)

# and finally this will run moc:
qt5_wrap_cpp(KTW_MOC_SRCS ${KTW_MOC_HDRS})

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
include_directories(${CMAKE_BINARY_DIR})

find_package(KRB5 REQUIRED krb5)
target_include_directories(krb5-ticket-watcher PRIVATE ${KRB5_INCLUDE_DIRS})


find_program(CREATEPOT createPot ${CMAKE_SOURCE_DIR}/po)
if (NOT CREATEPOT)
  message(FATAL_ERROR "createPot not found")
endif (NOT CREATEPOT)

add_custom_target(pot ${CREATEPOT} ${CMAKE_SOURCE_DIR}/po
                  DEPENDS krb5-ticket-watcher)
add_custom_target(backupclean find ${CMAKE_SOURCE_DIR} -name "*~" -print0 | xargs -0 rm -f)

# here we instruct CMake to build "krb5-ticket-watcher" executable from all of the source files
target_sources(krb5-ticket-watcher PRIVATE ${KTW_SRCS} ${KTW_MOC_SRCS} ${KTW_RC_SRCS} ${KTW_UI_HDRS})
target_link_libraries(krb5-ticket-watcher PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
target_link_libraries(krb5-ticket-watcher PRIVATE ${KRB5_LIBRARIES})
target_link_options(krb5-ticket-watcher PRIVATE -rdynamic)

install(TARGETS krb5-ticket-watcher RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
install(FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/)

add_subdirectory(po)

set(CPACK_SOURCE_IGNORE_FILES
    #svn files
    "\\\\.svn/"
    "\\\\.cvsignore$"
    # temporary files
    "\\\\.swp$"
    # backup files
    "~$"
    # eclipse files
    "\\\\.cdtproject$"
    "\\\\.cproject$"
    "\\\\.project$"
    "\\\\.settings/"
    # others
    "\\\\.#"
    "/#"
    "/build/"
    "/_build/"
    "/\\\\.git/"
    # used before
    "/CVS/"
    "/\\\\.libs/"
    "/\\\\.deps/"
    "\\\\.o$"
    "\\\\.lo$"
    "\\\\.la$"
    "Makefile\\\\.in$"
    )

set(CPACK_GENERATOR "TBZ2")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE}-${VERSION}")
include(CPack)

