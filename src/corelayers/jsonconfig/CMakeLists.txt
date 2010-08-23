qutim_add_plugin( jsonconfig
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/jsonconfigbackend.h
	EXTENSION_CLASS Core::JsonConfigBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "JSON config"
	DESCRIPTION "Default qutIM config implementation. Based on JSON."
)
