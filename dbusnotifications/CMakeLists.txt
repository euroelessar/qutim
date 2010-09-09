FIND_PACKAGE( Qt4 COMPONENTS QtDbus)
if(QT_QTDBUS_NOTFOUND)
	return()
endif()

# org.freedesktop.Notifications
qt4_add_dbus_interface(SRC
	org.freedesktop.Notifications.xml
	notification
)

qutim_add_plugin(dbusnotifications
	DISPLAY_NAME "Dbus notifications"
	DESCRIPTION "Notification system, based on Freedesktop"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARY}
	SOURCES ${SRC}
)

QUTIM_ADD_ARTWORK(dbusnotifications)
