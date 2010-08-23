qutim_add_plugin( contactlist
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplecontactlist.h
	EXTENSION_CLASS Core::SimpleContactList::Module
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple ContactList"
	DESCRIPTION "Default qutIM contact list implementation. Just simple"
)
