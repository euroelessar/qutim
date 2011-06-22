FIND_PACKAGE( Qt4 COMPONENTS QtDBus)
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "Cannot build DBusNotifications")
	return()
endif()

# org.freedesktop.Notifications
qt4_add_dbus_interface(DBUS_SRC
	org.freedesktop.Notifications.xml
	org_freedesktop_notification
)

qutim_add_plugin(dbusnotifications ${DBUS_SRC}
	DISPLAY_NAME "Dbus notifications"
	DESCRIPTION "Notification system, based on Freedesktop"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARY}
	INCLUDE_DIRS ${QT_QTDBUS_INCLUDE_DIR}
)
