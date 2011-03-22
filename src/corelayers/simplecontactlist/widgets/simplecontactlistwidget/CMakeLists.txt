qutim_add_plugin(simplecontactlistwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplewidget.h
	EXTENSION_CLASS Core::SimpleContactList::SimpleWidget
	DISPLAY_NAME "Simple"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Default qutIM contactlist widget implementation with global status button"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)