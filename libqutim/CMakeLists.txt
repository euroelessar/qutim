# libqutim project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libqutim )

# Search for source and headers in source directory (non-recursive)
FILE( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
FILE( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

# Add K8JSON code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.h" )

# Add QtIconLoader code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qticonloader/qticonloader.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qticonloader/qticonloader.h" )

# MinGW special options
# Not sure if those applicable for linux
if(MINGW)
    ADD_DEFINITIONS( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
    LIST( APPEND COMPILE_FLAGS "-mthreads" )
endif(MINGW)

# Require QT 4.6
SET( QT_MIN_VERSION "4.6.0" )

# Set QT modules
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTGUI FALSE )

# Search for QT4
FIND_PACKAGE( Qt4 REQUIRED )

# Include QT4
INCLUDE( ${QT_USE_FILE} )

# Add include directories
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../sdk02/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qticonloader/" )

# Generate moc files
QT4_WRAP_CPP( MOC_SRC ${HDR} )

# Add JDNS using separate cmake project
ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/jdns/" )

# This project will generate library
ADD_LIBRARY( libqutim SHARED ${SRC} ${MOC_SRC} )

# Set tagret properties
SET_TARGET_PROPERTIES ( libqutim PROPERTIES PREFIX "")

# Link with QT
TARGET_LINK_LIBRARIES( libqutim libjdns ${QT_LIBRARIES} )
