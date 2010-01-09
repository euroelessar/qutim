cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
if( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
endif(COMMAND cmake_policy )
project( meta-qutim )
set(QT_MIN_VERSION "4.6.0")

set( QUTIM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core" )
set( QUTIM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/core/libqutim/include" )
set( QUTIM_LIBRARY libqutim )

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/core/" "${CMAKE_CURRENT_BINARY_DIR}/core" )

file( GLOB PROTOCOLS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/protocols/" "${CMAKE_CURRENT_SOURCE_DIR}/protocols/*" )

foreach( PROTOCOL ${PROTOCOLS} )
	if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/protocols/${PROTOCOL}" )
		string( TOUPPER ${PROTOCOL} PROTOCOL_NAME)
		if( NOT DEFINED "NO_${PROTOCOL_NAME}" )
			message( "+ protocol ${PROTOCOL_NAME} added to build" )
			add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/protocols/${PROTOCOL}" "${CMAKE_CURRENT_BINARY_DIR}/protocols/${PROTOCOL}" ) 
		else( NOT DEFINED "NO_${PROTOCOL_NAME}" )
			message( "- protocol ${PROTOCOL_NAME} will be skipped" )
		endif( NOT DEFINED "NO_${PROTOCOL_NAME}" )
	endif( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/protocols/${PROTOCOL}" )
endforeach( PROTOCOL )

