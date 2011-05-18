qutim_add_plugin( xsettingsdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/xsettingslayerimpl.h
	EXTENSION_CLASS Core::XSettingsLayerImpl
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "X Settings dialog"
	DESCRIPTION "Default qutIM settings dialog implementation with OS X style top bar"
)
