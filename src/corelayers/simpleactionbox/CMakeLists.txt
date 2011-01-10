qutim_add_plugin( simpleactionbox
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simpleactionbox.h
	EXTENSION_CLASS Core::SimpleActionBoxGenerator
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "SimpleActionBox"
	DESCRIPTION "Simple actionbox module"
)
