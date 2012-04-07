CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Quetzal )

find_package( PkgConfig )
if( PKGCONFIG_FOUND )
	pkg_check_modules( PURPLE purple>=2.6.0 )
	include_directories( ${PURPLE_INCLUDE_DIRS} )
else( PKGCONFIG_FOUND )
	set( PURPLE_INCLUDE_DIR "" CACHE PATH "Path to libpurple include dir" )
	set( PURPLE_LIBRARY "" CACHE FILEPATH "Filepath to libpurple library" )
	set( GLIB_INCLUDE_DIR "" CACHE PATH "Path to glib-2.0 include dir" )
	set( GLIBCONFIG_INCLUDE_DIR "" CACHE PATH "Path to glibconfig.h file for glib-2.0" )
	set( GLIB_LIBRARY "" CACHE FILEPATH "Filepath to glib-2.0 library" )
	if (PURPLE_INCLUDE_DIR AND PURPLE_LIBRARY AND GLIB_INCLUDE_DIR AND GLIBCONFIG_INCLUDE_DIR AND GLIB_LIBRARY)
		set(PURPLE_FOUND true)
	endif()
	include_directories( ${PURPLE_INCLUDE_DIR} ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR} )
	list( APPEND PURPLE_LIBRARIES ${PURPLE_LIBRARY} ${GLIB_LIBRARY} )
endif( PKGCONFIG_FOUND )

IF (NOT PURPLE_FOUND)
	message( "Libpurple not found. Can't build quetzal" )
	return()
ENDIF()

SET( QT_USE_QTNETWORK TRUE )
ADD_DEFINITIONS( -DPURPLE_LIBDIR="${PURPLE_LIBDIR}" )
qutim_add_plugin( quetzal
	DISPLAY_NAME "Quetzal"
	DESCRIPTION "'Quetzal' is set of protocols, powered by libpurple"
	LINK_LIBRARIES "${PURPLE_LIBRARIES}"
	GROUP "Protocols"
)
