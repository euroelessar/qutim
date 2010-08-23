qutim_add_plugin( servicechooser
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/servicechooser.h
	EXTENSION_CLASS Core::ServiceChooser,StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Service chooser"
	DESCRIPTION "TODO"
)
