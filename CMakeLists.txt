# qutIM KDE Integration

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( KDEIntegration )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( Qt4 REQUIRED )
find_package( KDE4 REQUIRED )
include( KDE4Defaults )

macro_ensure_version( "4.2.0" "${GENERIC_LIB_VERSION}" KDE_HAS_4_2 )
if( NOT KDE_HAS_4_2 )
	message( SEND_ERROR "Minimum kdelibs version is 4.2.0, but ${GENERIC_LIB_VERSION} found" )
endif( NOT KDE_HAS_4_2 )

include( ${CMAKE_ROOT}/Modules/UseQt4.cmake )
find_path( QUTIM_INCLUDE_DIRS qutim/layerinterface.h ../../include )
if( QUTIM_INCLUDE_DIRS )
	message( STATUS "Found qutIM headers: ${QUTIM_INCLUDE_DIRS}" )
	include_directories( ${QUTIM_INCLUDE_DIRS} )
else( QUTIM_INCLUDE_DIRS )
	message( SEND_ERROR "Could NOT find qutIM development headers" )
endif( QUTIM_INCLUDE_DIRS )
if( APPLE )
	include_directories( /opt/local/include )
endif( APPLE )

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories( ${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${KDE4_INCLUDES}
	 )

add_subdirectory( emoticons )
add_subdirectory( speller )
add_subdirectory( crash )
add_subdirectory( notification )
add_subdirectory( phonon )