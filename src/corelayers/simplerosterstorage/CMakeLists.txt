qutim_add_plugin( simplerosterstorage
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplerosterstorage.h
	EXTENSION_CLASS Core::SimpleRosterStorage
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple roster storage"
	DESCRIPTION "Stores roster during offline at local storage"
)
