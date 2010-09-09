CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0011 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT ( Jabber )
SET ( QT_MIN_VERSION "4.6.0" )

SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/" )


if (GLOOX_EXTERNAL)
	SET( GLOOX_REQUIRED )
	include( FindGloox )
	if( GLOOX_FOUND )
		ADD_DEFINITIONS( -DHAVE_GLOOX )
		LIST(APPEND gloox ${GLOOX_LIBRARIES} )
		LIST(APPEND INCLUDE_DIRS ${GLOOX_INCLUDE_DIR})
	endif( GLOOX_FOUND )
else (GLOOX_EXTERNAL)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/gloox/" "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/gloox/")
	set ( gloox_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/gloox/src/" )
	set ( gloox_include_dir "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/gloox/include/gloox/" )
	file( MAKE_DIRECTORY ${gloox_include_dir} )
	file( GLOB GLOOX_HEADERS "${gloox_src_dir}*.h" )
	foreach( HEADERS_GENERATE ${GLOOX_HEADERS} )
		string( REPLACE ${gloox_src_dir} "" HEADER_FILE ${HEADERS_GENERATE} )
		file( RELATIVE_PATH HEADER_PATH ${gloox_include_dir} ${HEADERS_GENERATE} )
		if ( NOT EXISTS "${gloox_include_dir}${HEADER_FILE}" )
			file ( WRITE "${gloox_include_dir}${HEADER_FILE}" "#include \"${HEADER_PATH}\"" )
		endif ()
	endforeach( HEADERS_GENERATE )
	LIST (APPEND INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/gloox/include/")
endif(GLOOX_EXTERNAL)

SET( QT_USE_QTNETWORK TRUE )
set ( USE_TESTS OFF )

ADD_DEFINITIONS( -DGLOOX_IMPORTS )

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on gloox library"
	GROUP "Protocols"
	LINK_LIBRARIES gloox
	INCLUDE_DIRS "${INCLUDE_DIRS}"
)
