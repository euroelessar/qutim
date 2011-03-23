if(NOT SYMBIAN)
	message(STATUS "Symbian contactlist widget will be skipped")
	return()
endif()

qutim_add_plugin(symbiancontactlistwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/symbianwidget.h
	EXTENSION_CLASS Core::SimpleContactList::SymbianWidget
	DISPLAY_NAME "Symbian"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Symbian specific qutIM contactlist widget implementation"
	DEPENDS symbiancontactlist
	LINK_LIBRARIES simplecontactlist
)