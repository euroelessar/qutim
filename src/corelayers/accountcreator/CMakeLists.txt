qutim_add_plugin( accountcreator
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/accountcreator.h
	EXTENSION_CLASS Core::AccountCreator,StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Account creator"
	DESCRIPTION "Default qutIM account creator"
)
