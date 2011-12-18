FIND_PACKAGE(PkgConfig)
pkg_check_modules(DBUSMENUQT dbusmenu-qt>=0.8.1)

find_package(Qt4 COMPONENTS QtDBus )
if(NOT QT_QTDBUS_FOUND OR NOT DBUSMENUQT_FOUND)
	message(STATUS "Cannot build unity launcher integration")
	return()
endif(NOT QT_QTDBUS_FOUND OR NOT DBUSMENUQT_FOUND)

qutim_add_plugin( "unitylauncher"
	DISPLAY_NAME "UnityLauncher"
	DESCRIPTION "Ubuntu Unity panel integration"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
	QTLIBRARIES DBUS
        LINK_LIBRARIES	${DBUSMENUQT_LIBRARIES}
	INCLUDE_DIRS ${DBUSMENUQT_INCLUDE_DIRS}
)
