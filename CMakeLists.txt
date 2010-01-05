CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0011 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT ( Jabber )
SET ( QT_MIN_VERSION "4.6.0" )

SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/" )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/" "${CMAKE_CURRENT_BINARY_DIR}/libs/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/")

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")
if(QUTIM_PATH)
    set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
else(QUTIM_PATH)
    set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

SET( QT_USE_QTNETWORK TRUE )

if ( WIN32 )
ADD_DEFINITIONS( -DGLOOX_IMPORTS )
endif ( WIN32 )

include( ${QUTIM_CMAKE_PLUGIN} )
ADD_QUTIMPLUGIN_EXT2( "jabber" "src" "" gloox )
