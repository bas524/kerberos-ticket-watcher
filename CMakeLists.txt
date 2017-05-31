# set project's name
PROJECT( krb5-ticket-watcher )
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
cmake_minimum_required(VERSION 2.6)
  

SET( PACKAGE krb5-ticket-watcher )
SET( VERSION 1.0.3 )


# with SET() command you can change variables or define new ones
# here we define KTW_SRCS variable that contains a list of all .cpp files
# note that we don't need \ at the end of line
SET( KTW_SRCS
     src/kinitdialog.cpp
     src/krb5ticketwatcher.cpp
     src/main.cpp
     src/v5.cpp
)
  
# another list, this time it includes all header files that should be treated with moc
SET( KTW_MOC_HDRS
     src/krb5ticketwatcher.h 
     src/kinitdialog.h 
     src/pwdialog.h 
     src/pwchangedialog.h
)
  
# some .ui files
SET( KTW_UIS
     src/pwdialog.ui 
     src/pwchangedialog.ui 
     src/mainwidget.ui 
     src/kinitdialog.ui
)
  
  
# enable warnings
ADD_DEFINITIONS( -Wall -DTEXTDOMAIN=krb5-ticket-watcher)
#ADD_DEFINITIONS( -DDEBUG )
  
# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET( QT_USE_QTNETWORK TRUE )   
  
# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )
  
FIND_PACKAGE( Gettext REQUIRED )
IF (GETTEXT_FOUND)
  MESSAGE(STATUS "Found Gettext: ${GETTEXT_SOURCE}")
  INCLUDE_DIRECTORIES(${GETTEXT_INCLUDE_DIR})
ELSE (GETTEXT_FOUND)
  MESSAGE( FATAL_ERROR "Gettext not found" )
ENDIF (GETTEXT_FOUND)

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )
  
# this will run uic on .ui files:
QT4_WRAP_UI( KTW_UI_HDRS ${KTW_UIS} OPTIONS -tr ki18n)
  
# and finally this will run moc:
QT4_WRAP_CPP( KTW_MOC_SRCS ${KTW_MOC_HDRS} )
  
# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )


FIND_PROGRAM (KRB5-CONFIG krb5-config /usr/lib/mit/bin/)
IF (NOT KRB5-CONFIG)
      MESSAGE(FATAL_ERROR "krb5-config not found")
ENDIF (NOT KRB5-CONFIG)

EXECUTE_PROCESS(COMMAND ${KRB5-CONFIG} --libs krb5 OUTPUT_VARIABLE KRB5-LIBS RESULT_VARIABLE RET)

SET(LIBS "")
IF (NOT RET)
      STRING(STRIP ${KRB5-LIBS} KRB5LIBS)
      set (LIBS ${LIBS} ${KRB5LIBS})
ELSE(NOT RET)
      MESSAGE(STATUS "krb5 libs not found ${KRB5-LIBS} ${RET}")
ENDIF (NOT RET)

FIND_PROGRAM (CREATEPOT createPot ${CMAKE_SOURCE_DIR}/po)
IF (NOT CREATEPOT)
      MESSAGE(FATAL_ERROR "createPot not found")
ENDIF (NOT CREATEPOT)
 
ADD_CUSTOM_TARGET(pot ${CREATEPOT} ${CMAKE_SOURCE_DIR}/po
                  DEPENDS krb5-ticket-watcher)
ADD_CUSTOM_TARGET(backupclean find ${CMAKE_SOURCE_DIR} -name "*~" -print0 | xargs -0 rm -f )

# here we instruct CMake to build "krb5-ticket-watcher" executable from all of the source files
ADD_EXECUTABLE( krb5-ticket-watcher ${KTW_SRCS} ${KTW_MOC_SRCS} ${KTW_RC_SRCS} ${KTW_UI_HDRS} )
  
SET (LIBS ${LIBS} ${QT_LIBRARIES})

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
TARGET_LINK_LIBRARIES( krb5-ticket-watcher ${LIBS} )

INSTALL( TARGETS krb5-ticket-watcher RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL( FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/ )
INSTALL( FILES ${CMAKE_SOURCE_DIR}/krb5-ticket-watcher.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/ )

ADD_SUBDIRECTORY( po )

SET (CPACK_SOURCE_IGNORE_FILES
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

SET( CPACK_GENERATOR "TBZ2")
SET( CPACK_SOURCE_GENERATOR "TBZ2")
SET( CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE}-${VERSION}" )
INCLUDE(CPack)

