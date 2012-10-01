qutim_add_plugin( servicechooser
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/servicechooser.h
	EXTENSION_CLASS Core::ServiceChoooser,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Service chooser"
	DESCRIPTION "TODO"
)
