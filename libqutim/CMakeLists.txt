# libqutim project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libqutim )

# Search for source and headers in source directory (non-recursive)
FILE( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
FILE( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB SRC_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp" )
FILE( GLOB HDR_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.h" )

LIST( APPEND SRC ${SRC_GAME} )
LIST( APPEND HDR ${HDR_GAME} )

# Add K8JSON code
ADD_DEFINITIONS( -DK8JSON_INCLUDE_GENERATOR -DK8JSON_INCLUDE_COMPLEX_GENERATOR )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.h" )

# Add qtdwm code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.h" )


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

option(WITH_DOXYGEN "" ON)

if(WITH_DOXYGEN)
    message("-- Checking for Doxygen...")
    find_package(Doxygen)
endif()

# Include QT4
INCLUDE( ${QT_USE_FILE} )

# Add include directories
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../sdk02/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qticonloader/" )

# Generate moc files
QT4_WRAP_CPP( MOC_SRC ${HDR} )

# Add JDNS using separate cmake project
ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/jdns/" )
# Add Aero theme integration for Windows Vista and later
# ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtdwm/")

# This project will generate library
ADD_LIBRARY( libqutim SHARED ${SRC} ${MOC_SRC} ${HDR} )

# Set tagret properties
SET_TARGET_PROPERTIES ( libqutim PROPERTIES PREFIX "")

IF(APPLE)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(CORE_SERVICES_LIBRARY CoreServices )
   MARK_AS_ADVANCED (CARBON_LIBRARY
					 CORE_SERVICES_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${APP_SERVICES_LIBRARY})
ENDIF (APPLE)

# Link with QT
TARGET_LINK_LIBRARIES( libqutim libjdns ${QT_LIBRARIES} ${EXTRA_LIBS})


INSTALL (TARGETS libqutim DESTINATION "lib")

if(DOXYGEN_FOUND)
    set(DOC_TARGET "doc")
    configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    add_custom_target(${DOC_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qutim)
    add_dependencies(libqutim ${DOC_TARGET})
endif()

