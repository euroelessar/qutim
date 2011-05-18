qutim_add_plugin( soundthemeselector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/soundthemesettings.h
	EXTENSION_CLASS Core::SoundThemeSettings,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Sound theme selector"
	DESCRIPTION "Simple sound theme selector"
)
