qutim_add_plugin( notificationssettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/notificationssettings.h
	EXTENSION_CLASS Core::NotificationsSettings
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Notifications settings"
	DESCRIPTION "Default settings widget set for notifications"
)
