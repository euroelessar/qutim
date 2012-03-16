find_package( PkgConfig )
pkg_check_modules( DBUSMENUQT dbusmenu-qt>=0.6.2 )

if( NOT DBUSMENUQT_FOUND )
	message(STATUS "Cannot build unity launcher integration")
	return()
endif()

qutim_add_plugin( "unitylauncher"
	DISPLAY_NAME "UnityLauncher"
	DESCRIPTION "Ubuntu Unity panel integration"
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src"
	QT_LIBRARIES DBUS
	LINK_LIBRARIES	${DBUSMENUQT_LIBRARIES}
	INCLUDE_DIRS ${DBUSMENUQT_INCLUDE_DIRS}
)
