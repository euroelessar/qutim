qutim_add_plugin( trayicon
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simpletray.h
	EXTENSION_CLASS Core::SimpleTray
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple tray"
	DESCRIPTION "Default qutIM system tray icon implementation"
)
