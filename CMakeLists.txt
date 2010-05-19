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
endif ( ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR} )
if(QUTIM_PATH)
    set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
else(QUTIM_PATH)
    set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

list( APPEND KDEINT_LIBS ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} )

include( ${QUTIM_CMAKE_PLUGIN} )

ADD_QUTIMPLUGIN_EXT2( "kdeintegration" "src" "" ${KDEINT_LIBS})

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
