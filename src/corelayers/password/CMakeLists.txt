qutim_add_plugin( password
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplepassworddialog.h
	EXTENSION_CLASS Core::SimplePasswordDialog
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple password dialog"
	DESCRIPTION "Default qutIM password dialog"
)
