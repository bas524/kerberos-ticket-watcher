# set project's name
  PROJECT( krb5-ticket-watcher )
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
INCLUDE( createPot )
cmake_minimum_required(VERSION 2.6)
  
  # with SET() command you can change variables or define new ones
  # here we define SAMPLE_SRCS variable that contains a list of all .cpp files
  # note that we don't need \ at the end of line
  SET( SAMPLE_SRCS
src/kinitdialog.cpp
src/krb5ticketwatcher.cpp
src/main.cpp
src/v5.cpp
  )
  
  # another list, this time it includes all header files that should be treated with moc
  SET( SAMPLE_MOC_HDRS
           src/krb5ticketwatcher.h 
           src/kinitdialog.h 
           src/pwdialog.h 
           src/pwchangedialog.h
  )
  
  # some .ui files
  SET( SAMPLE_UIS
          src/pwdialog.ui 
          src/pwchangedialog.ui 
          src/mainwidget.ui 
          src/kinitdialog.ui
  )
  
  # and finally an resource file
  #SET( SAMPLE_RCS
  #     ./src/rc/sample.qrc
  #)
  
  # enable warnings
  ADD_DEFINITIONS( -Wall -DDEBUG -DTEXTDOMAIN="krb5-ticket-watcher")
  
  # by default only QtCore and QtGui modules are enabled
  # other modules must be enabled like this:
  SET( QT_USE_QTNETWORK TRUE )   
  
  # this command finds Qt4 libraries and sets all required variables
  # note that it's Qt4, not QT4 or qt4
  FIND_PACKAGE( Qt4 REQUIRED )
  
  FIND_PACKAGE( Gettext REQUIRED )

  createPot()

  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
  INCLUDE( ${QT_USE_FILE} )
  
  # this command will generate rules that will run rcc on all files from SAMPLE_RCS
  # in result SAMPLE_RC_SRCS variable will contain paths to files produced by rcc
  #QT4_ADD_RESOURCES( SAMPLE_RC_SRCS ${SAMPLE_RCS} )
  
  # this will run uic on .ui files:
  QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )
  
  # and finally this will run moc:
  QT4_WRAP_CPP( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS} )
  
  # we need this to be able to include headers produced by uic in our code
  # (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
  INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )


  # here we instruct CMake to build "sample" executable from all of the source files
  ADD_EXECUTABLE( krb5-ticket-watcher ${SAMPLE_SRCS} ${SAMPLE_MOC_SRCS} ${SAMPLE_RC_SRCS} ${SAMPLE_UI_HDRS} )
  
  # last thing we have to do is to tell CMake what libraries our executable needs,
  # luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
  TARGET_LINK_LIBRARIES( krb5-ticket-watcher ${QT_LIBRARIES} -lkrb5 -lcom_err )

 
  ADD_SUBDIRECTORY( gpo EXCLUDE_FROM_ALL )
