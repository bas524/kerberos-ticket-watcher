set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/modules")
cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0075 NEW)

set(MAJOR "1")
set(MINOR "0")
set(PATCH "5")
cmake_policy(SET CMP0048 NEW)
project(krb5-ticket-watcher VERSION ${MAJOR}.${MINOR}.${PATCH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(VersionHeader)
set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}.${COMMITTER_SHORTSHA}")

message(STATUS "COMMITTER_FULLSHA ${COMMITTER_FULLSHA}")
message(STATUS "COMMITTER_SHORTSHA ${COMMITTER_SHORTSHA}")
message(STATUS "COMMITTER_DATE ${COMMITTER_DATE}")
message(STATUS "PROJECT_VERSION ${PROJECT_VERSION}")

include_directories(src)

add_executable(krb5-ticket-watcher)

target_compile_definitions(krb5-ticket-watcher PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

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
    src/keychainclass.cpp
    src/KtwOptions.cpp
    src/KtwOptions.h
    src/pwdialog.cpp
    src/pwchangedialog.cpp
    src/pwdialog.h
    src/pwchangedialog.h
    src/ldapinfo.h
    src/ldapinfo.cpp
    )

# another list, this time it includes all header files that should be treated with moc
set(KTW_MOC_HDRS
    src/krb5ticketwatcher.h
    src/kinitdialog.h
    src/pwdialog.h
    src/pwchangedialog.h
    src/keychainclass.h
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


set(QT_VER Qt5)
find_package(Qt5 COMPONENTS Core Widgets Gui Network Keychain REQUIRED)
# this will run uic on .ui files:
qt5_wrap_ui(KTW_UI_HDRS ${KTW_UIS} OPTIONS -tr ki18n)
# and finally this will run moc:
qt5_wrap_cpp(KTW_MOC_SRCS ${KTW_MOC_HDRS})
#add resources
qt5_add_resources(KTW_RC_SRCS rc/icons.qrc)

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
  message(STATUS "Found Gettext: ${GETTEXT_SOURCE}")
  include_directories(${GETTEXT_INCLUDE_DIR})
else (GETTEXT_FOUND)
  message(FATAL_ERROR "Gettext not found")
endif (GETTEXT_FOUND)

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
include_directories(${CMAKE_BINARY_DIR})

find_package(KRB5 REQUIRED krb5)
target_include_directories(krb5-ticket-watcher PRIVATE ${KRB5_INCLUDE_DIRS})
target_include_directories(krb5-ticket-watcher PRIVATE ${QTKEYCHAIN_INCLUDE_DIRS})


find_program(CREATEPOT createPot ${CMAKE_SOURCE_DIR}/po)
if (NOT CREATEPOT)
  message(FATAL_ERROR "createPot not found")
endif (NOT CREATEPOT)

add_custom_target(pot ${CREATEPOT} ${CMAKE_SOURCE_DIR}/po DEPENDS krb5-ticket-watcher)
add_custom_target(backupclean find ${CMAKE_SOURCE_DIR} -name "*~" -print0 | xargs -0 rm -f)

# here we instruct CMake to build "krb5-ticket-watcher" executable from all of the source files
target_sources(krb5-ticket-watcher PRIVATE ${KTW_SRCS} ${KTW_MOC_SRCS} ${KTW_RC_SRCS} ${KTW_UI_HDRS})
target_link_libraries(krb5-ticket-watcher PRIVATE ${QT_VER}::Core ${QT_VER}::Gui ${QT_VER}::Widgets ${QT_VER}::Network)
target_link_libraries(krb5-ticket-watcher PRIVATE ${KRB5_LIBRARIES})
target_link_libraries(krb5-ticket-watcher PRIVATE ${QTKEYCHAIN_LIBRARIES})
message(STATUS "QTKEYCHAIN_LIBRARIES ${QTKEYCHAIN_LIBRARIES}")
target_link_options(krb5-ticket-watcher PRIVATE -rdynamic)
add_link_options($<$<CONFIG:RELEASE>:-s>)

if (APPLE)
  target_include_directories(krb5-ticket-watcher PRIVATE /usr/local/opt/gettext/include)
  target_link_directories(krb5-ticket-watcher PRIVATE /usr/local/opt/gettext/lib)
  target_link_libraries(krb5-ticket-watcher PRIVATE -lintl)
endif ()

add_subdirectory(po)

install(TARGETS krb5-ticket-watcher RUNTIME DESTINATION bin COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.desktop DESTINATION share/applications/ COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.png DESTINATION share/pixmaps/ COMPONENT application)

set(CPACK_SOURCE_IGNORE_FILES
    #svn files
    "\\\\.svn/"
    "\\\\.cvsignore$"
    #git files
    "\\\\.git/"
    "\\\\.github/"
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


set(CPACK_PACKAGE_NAME "krb5-ticket-watcher")
set(CPACK_PACKAGE_VERSION "${MAJOR}.${MINOR}.${PATCH}")
set(CPACK_PACKAGE_VENDOR "community")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/bas524/kerberos-ticket-watcher")
set(CPACK_PACKAGE_CONTACT "bas524")
set(CPACK_PACKAGE_DESCRIPTION "krb5-ticket-watcher is a tray applet for watching, renewing, and reinitializing Kerberos tickets.
It's very useful in corporate network for linux-workstations which should be in the AD-domain.")

set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_SECTION "net")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CMAKE_PROJECT_HOMEPAGE_URL})
set(CPACK_DEBIAN_PACKAGE_LICENSE "GPLv2.0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "krb5-user, krb5-config, libqt5core5a, libqt5gui5, libqt5widgets5, libqt5network5, libicu66, libqt5keychain1")

set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_RPM_PACKAGE_SECTION "net")
set(CPACK_RPM_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL})
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2.0")
set(CPACK_RPM_PACKAGE_REQUIRES "krb5-config, qt5-qtbase, qt5-qtnetworkauth, qtkeychain-qt5, libicu")

include(CPack)
cpack_add_component(application DISPLAY_NAME "Kerberos Ticket Watcher App" REQUIRED)
cpack_add_component(localization DISPLAY_NAME "Kerberos Ticket Watcher Localization " REQUIRED)
