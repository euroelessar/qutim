qutim_add_plugin( settings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/chatsettings.h
	EXTENSION_CLASS "Core::ChatSettings, StartupModule"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Chat settings"
	DESCRIPTION ""
)
