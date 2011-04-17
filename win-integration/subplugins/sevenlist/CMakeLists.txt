qutim_add_plugin(sevenwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/sevenwidget.h
	EXTENSION_CLASS Core::SimpleContactList::SimpleWidget
	DISABLED "TRUE"
	DISPLAY_NAME "Seven"
	GROUP "Windows Seven Integration"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Windows seven specific implementation with button's in caption"
	DEPENDS symbiancontactlist
	LINK_LIBRARIES simplecontactlist
)
