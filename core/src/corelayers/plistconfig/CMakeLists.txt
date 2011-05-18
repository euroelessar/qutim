qutim_add_plugin( plistconfig
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/plistconfigbackend.h
	EXTENSION_CLASS Core::PListConfigBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "PList config"
	DESCRIPTION "Additional qutIM config implementation for Apple plists"
)
