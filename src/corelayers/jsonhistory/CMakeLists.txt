qutim_add_plugin( jsonhistory
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/jsonhistory.h
	EXTENSION_CLASS Core::JsonHistory
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Json History"
	DESCRIPTION "Default qutIM history implementation, based on JavaScript Object Notation"
)
