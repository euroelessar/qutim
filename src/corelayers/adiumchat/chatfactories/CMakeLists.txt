file( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

foreach( PLUGIN ${PLUGINS} )
	if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt" )
		string( TOUPPER ${PLUGIN} PLUGIN_NAME)
		option( ${PLUGIN_NAME} "" ON )
		if( ${PLUGIN_NAME} )
			message( "+ chatfactory '${PLUGIN_NAME}' added to build" )
			add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}" "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN}" )
		else( ${PLUGIN_NAME} )
			message( "- chatfactory '${PLUGIN_NAME}' will be skipped" )
		endif( ${PLUGIN_NAME} )
	endif( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt" )
endforeach( PLUGIN )

 
