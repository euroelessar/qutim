list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( LibAttica )
if( NOT LIBATTICA_FOUND )
	message( "LibAttica is not found. Can't build package manager" )
	return()
endif()

include_directories( ${LIBATTICA_INCLUDE_DIR}
	"${CMAKE_CURRENT_BINARY_DIR}/include"
)

if(MSVC)
        set(PLUGMAN_CFLAGS "/Zc:wchar_t-")
endif()

qutim_add_plugin(plugman
	DISPLAY_NAME "Package Manager"
	DESCRIPTION "Provides possibility to Get Hot New Stuff from open community"
        LINK_LIBRARIES ${LIBATTICA_LIBRARIES}
	QT_LIBRARIES DECLARATIVE
        COMPILE_FLAGS ${PLUGMAN_CFLAGS}
)

qutim_add_artwork_dir(qml)
