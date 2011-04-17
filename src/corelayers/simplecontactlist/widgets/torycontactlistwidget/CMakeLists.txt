qutim_add_plugin(torycontactlistwidget
	GROUP "ContactWidget"
	DISABLED TRUE
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/torywidget.h
	EXTENSION_CLASS Core::SimpleContactList::ToryWidget
	DISPLAY_NAME "Tory Contactlist"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Tory contactlist"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
