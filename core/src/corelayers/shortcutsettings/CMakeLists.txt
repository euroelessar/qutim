qutim_add_plugin( shortcutsettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/shortcutsettings.h
	EXTENSION_CLASS Core::ShortcutSettings,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Shortcut settings"
	DESCRIPTION ""
)
