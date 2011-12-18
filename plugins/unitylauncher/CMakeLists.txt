FIND_PACKAGE(PkgConfig)
pkg_check_modules(DBUSMENUQT dbusmenu-qt>=0.8.1)

qutim_add_plugin( "unitylauncher"
	DISPLAY_NAME "UnityLauncher"
	DESCRIPTION "Ubuntu Unity panel integration"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
	QT_LIBRARIES DBUS
	LINK_LIBRARIES ${DBUSMENUQT_LIBRARIES}
	INCLUDE_DIRS ${DBUSMENUQT_INCLUDE_DIRS}
)
