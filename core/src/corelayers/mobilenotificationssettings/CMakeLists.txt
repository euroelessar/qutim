qutim_add_plugin( mobilenotificationssettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/mobilenotificationssettings.h
	EXTENSION_CLASS Core::MobileNotificationsSettings
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mobile notifications settings"
	DESCRIPTION "Mobile settings widget set for notifications"
)
