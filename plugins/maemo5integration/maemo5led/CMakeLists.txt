qutim_add_plugin(maemo5led
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Maemo 5 Led Notifications"
	DESCRIPTION "Notification system, based Maemo 5 Led"
	QT_LIBRARIES DBUS
)
