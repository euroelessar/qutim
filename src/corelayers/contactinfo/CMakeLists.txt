qutim_add_plugin( contactinfo
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/contactinfo.h
	EXTENSION_CLASS Core::ContactInfo
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple information window about a contact"
	DESCRIPTION "Default qutIM implementation of the information window"
)
