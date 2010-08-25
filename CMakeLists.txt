cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
if( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
endif(COMMAND cmake_policy )
project( meta-qutim )
set(QT_MIN_VERSION "4.6.0")

set( QUTIM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core" CACHE FILEPATH "Path to qutim sources")
set( QUTIM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/core/libqutim/include" CACHE FILEPATH "Path to qutim headers")
set( QUTIM_LIBRARY libqutim CACHE FILEPATH "Path to qutim library")

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/core/" "${CMAKE_CURRENT_BINARY_DIR}/core" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/protocols/" "${CMAKE_CURRENT_BINARY_DIR}/protocols" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/translations/" "${CMAKE_CURRENT_BINARY_DIR}/translations" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/integrations/" "${CMAKE_CURRENT_BINARY_DIR}/integrations" )

set( PLUGIN_DIRS
	"plugins" )

foreach( PLUGIN_DIR ${PLUGIN_DIRS} )
	file( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/*" )

	foreach( PLUGIN ${PLUGINS} )
		if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/${PLUGIN}/CMakeLists.txt )
			string( TOUPPER ${PLUGIN} PLUGIN_NAME)
			if( NOT DEFINED "NO_${PLUGIN_NAME}" )
				message( "+ plugin ${PLUGIN_NAME} added to build" )
				add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/${PLUGIN}" "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_DIR}/${PLUGIN}" ) 
			else( NOT DEFINED "NO_${PLUGIN_NAME}" )
				message( "- plugin ${PLUGIN_NAME} will be skipped" )
			endif( NOT DEFINED "NO_${PLUGIN_NAME}" )
		endif( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/${PLUGIN}/CMakeLists.txt )
	endforeach( PLUGIN )
endforeach( PLUGIN_DIR )
