qutim_add_plugin(symbiancontactlistwidget
	GROUP "Symbian integration Plugin"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/symbianwidget.h
	EXTENSION_CLASS Core::SimpleContactList::SymbianWidget
	DISPLAY_NAME "Symbian"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Symbian specific qutIM contactlist widget implementation"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
