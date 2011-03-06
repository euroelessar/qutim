qutim_add_plugin( simpleaboutdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simpleaboutcreator.h
	EXTENSION_CLASS Core::SimpleAboutCreator
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple about dialog"
	DESCRIPTION "Simple dialog about qutIM and it's authors"
)
