# set project's name
project(krb5-ticket-watcher)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
cmake_minimum_required(VERSION 2.6)


set(PACKAGE krb5-ticket-watcher)
set(VERSION 1.0.3)


# with SET() command you can change variables or define new ones
# here we define KTW_SRCS variable that contains a list of all .cpp files
# note that we don't need \ at the end of line
include_directories(src)

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
#ADD_DEFINITIONS( -DDEBUG )

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
set(QT_USE_QTNETWORK TRUE)

# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
  message(STATUS "Found Gettext: ${GETTEXT_SOURCE}")
  include_directories(${GETTEXT_INCLUDE_DIR})
else (GETTEXT_FOUND)
  message(FATAL_ERROR "Gettext not found")
endif (GETTEXT_FOUND)

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)

# this will run uic on .ui files:
qt5_wrap_ui(KTW_UI_HDRS ${KTW_UIS} OPTIONS -tr ki18n)

# and finally this will run moc:
qt5_wrap_cpp(KTW_MOC_SRCS ${KTW_MOC_HDRS})

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
include_directories(${CMAKE_BINARY_DIR})


find_program(KRB5-CONFIG krb5-config /usr/lib/mit/bin/)
if (NOT KRB5-CONFIG)
  message(FATAL_ERROR "krb5-config not found")
endif (NOT KRB5-CONFIG)

execute_process(COMMAND ${KRB5-CONFIG} --libs krb5 OUTPUT_VARIABLE KRB5-LIBS RESULT_VARIABLE RET)

set(LIBS "")
if (NOT RET)
  string(STRIP ${KRB5-LIBS} KRB5LIBS)
  set(LIBS ${LIBS} ${KRB5LIBS})
else (NOT RET)
  message(STATUS "krb5 libs not found ${KRB5-LIBS} ${RET}")
endif (NOT RET)

find_program(CREATEPOT createPot ${CMAKE_SOURCE_DIR}/po)
if (NOT CREATEPOT)
  message(FATAL_ERROR "createPot not found")
endif (NOT CREATEPOT)

add_custom_target(pot ${CREATEPOT} ${CMAKE_SOURCE_DIR}/po
                  DEPENDS krb5-ticket-watcher)
add_custom_target(backupclean find ${CMAKE_SOURCE_DIR} -name "*~" -print0 | xargs -0 rm -f)

# here we instruct CMake to build "krb5-ticket-watcher" executable from all of the source files
add_executable(krb5-ticket-watcher ${KTW_SRCS} ${KTW_MOC_SRCS} ${KTW_RC_SRCS} ${KTW_UI_HDRS})
qt5_use_modules(krb5-ticket-watcher Core Gui Widgets Network)

set(LIBS ${LIBS} ${QT_LIBRARIES})

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
target_link_libraries(krb5-ticket-watcher ${LIBS})

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

