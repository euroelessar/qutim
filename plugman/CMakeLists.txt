list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( LibAttica )
if( NOT LIBATTICA_FOUND )
	message( "LibAttica is not found. Can't build package manager" )
	return()
endif()

include_directories( ${LIBATTICA_INCLUDE_DIR} )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quasar" )

qutim_add_plugin( package_manager
	DISPLAY_NAME "Package Manager"
	DESCRIPTION "Provides possibility to Get Hot New Stuff from open community"
	LINK_LIBRARIES ${LIBATTICA_LIBRARIES} quasar
)
