qutim_add_plugin( oldsoundtheme
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/oldsoundthemebackend.h
	EXTENSION_CLASS Core::Core::OldSoundThemeBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "qutIM sound themes"
	DESCRIPTION "Default qutIM sound theme engine"
)
