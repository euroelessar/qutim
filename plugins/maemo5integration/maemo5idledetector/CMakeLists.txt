qutim_add_plugin( maemo5idledetector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/maemo5idle.h
	EXTENSION_CLASS Maemo5Idle
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Maemo 5 Idle detector"
	DESCRIPTION "Native Maemo 5 idle detector"
	QT_LIBRARIES DBUS
)
