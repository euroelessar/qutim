qutim_add_plugin(maccontactlistwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/macwidget.h
	EXTENSION_CLASS Core::SimpleContactList::MacWidget
	DISPLAY_NAME "Mac Contactlist"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Mac OS X contactlist"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
