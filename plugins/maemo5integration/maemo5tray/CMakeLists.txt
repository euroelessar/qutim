INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(GTK gtk+-2.0)
PKG_CHECK_MODULES(DBUS dbus-1)
PKG_CHECK_MODULES(DBUS-GLIB dbus-glib-1)

if(NOT GTK_FOUND)
	message(STATUS "Cannot build Maemo 5 Tray integration")
	return()
endif()

if(NOT DBUS_FOUND)
	message(STATUS "Cannot build Maemo 5 Tray integration")
	return()
endif()
if(NOT DBUS-GLIB_FOUND)
	message(STATUS "Cannot build Maemo 5 Tray integration")
	return()
endif()



qutim_add_plugin( maemo5tray
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/maemo5tray.h
	EXTENSION_CLASS Maemo5Tray
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Maemo 5 Tray "
	DESCRIPTION "Maemo 5 specific system tray implementation"
	LINK_LIBRARIES ${GTK_LIBRARIES} ${DBUS_LIBRARIES} ${DBUS-GLIB_LIBRARIES} 
	INCLUDE_DIRS ${GTK_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS} ${DBUS-GLIB_INCLUDE_DIRS}
)
