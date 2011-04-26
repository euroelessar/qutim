qutim_add_plugin( notificationfilter
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/notificationfilter.h
	EXTENSION_CLASS Core::NotificationFilterImpl,qutim_sdk_0_3::NotificationFilter,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Notification filter"
	DESCRIPTION "Notification filter"
)
