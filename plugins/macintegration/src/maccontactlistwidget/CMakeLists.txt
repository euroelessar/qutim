if(NOT APPLE)
	message(STATUS "Mac Os X contactlist widget will be skipped")
	return()
endif()


qutim_add_plugin(maccontactlistwidget
	GROUP "ContactWidget"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/macwidget.h
	EXTENSION_CLASS Core::SimpleContactList::MacWidget
	DISPLAY_NAME "Mac Contactlist"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Mac OS X contactlist"
	DEPENDS qutim-simplecontactlist
	LINK_LIBRARIES qutim-simplecontactlist
)
