# org.freedesktop.Notifications
qt4_add_dbus_interface(DBUS_SRC
	org.freedesktop.Notifications.xml
	org_freedesktop_notifications
)

qutim_add_plugin(dbusnotifications ${DBUS_SRC}
	DISPLAY_NAME "Dbus notifications"
	DESCRIPTION "Notification system, based on Freedesktop"
	QT_LIBRARIES DBUS
)
