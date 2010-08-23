qutim_add_plugin( searchdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/searchdialoglayer.h
	EXTENSION_CLASS Core::SearchLayer
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Search window"
	DESCRIPTION "Default qutIM implementation of search window"
)
