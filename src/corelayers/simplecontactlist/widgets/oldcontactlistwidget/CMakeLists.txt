return()

qutim_add_plugin(oldcontactlistwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/oldwidget.h
	EXTENSION_CLASS Core::SimpleContactList::OldWidget
	DISPLAY_NAME "Classic Widget"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Old qutIM style contactlist widget implementation"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
