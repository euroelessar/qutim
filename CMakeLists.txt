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

if ( ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR} )
	set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")
	set(QUTIM_BINARY_DIR "" CACHE FILEPATH "Path to qutim build directory")
	set(QUTIM_LIBRARY QUTIM_LIBRARY-NOTFOUND CACHE FILEPATH "Path to qutim library")
endif ( ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR} )
if( QUTIM_BINARY_DIR )
	set( QUTIM_INCLUDE_DIRS ${QUTIM_BINARY_DIR}/libqutim/include )
	set( QUTIM_LIBRARY_DIR ${QUTIM_BINARY_DIR}/libqutim/ )
endif( QUTIM_BINARY_DIR )

LIST (APPEND CMAKE_MODULE_PATH "cmake")
LIST (APPEND CMAKE_MODULE_PATH "${QUTIM_PATH}/cmake")
find_package( QutIM REQUIRED )

list( APPEND KDEINT_LIBS ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )

qutim_add_plugin( kdeintegration
	DISPLAY_NAME "KDE integration"
	DESCRIPTION "Integration with K Desktop Environment"
	LINK_LIBRARIES ${KDEINT_LIBS}
)

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )

include_directories( ${QT_QTGUI_INCLUDE_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${KDE4_INCLUDES}
	 )

#add_subdirectory( emoticons )
#add_subdirectory( speller )
#add_subdirectory( crash )
#add_subdirectory( notification )
#add_subdirectory( phonon )
