include_directories(.)

qutim_add_plugin(simplecontactlist
	EXTENSION
	EXTENSION_HEADER contactlist/simplecontactlist.h
	EXTENSION_CLASS Core::SimpleContactList::Module
	SOURCE_DIR contactlist
	DISPLAY_NAME "Simple ContactList"
	DESCRIPTION "Default qutIM contact list implementation. Just simple"
	COMPILE_FLAGS "-DSIMPLECONTACTLIST_LIBRARY"
)

qutim_add_plugin(simplecontactdelegate
	EXTENSION
	EXTENSION_HEADER simplecontactdelegate/contactdelegate.h
	EXTENSION_CLASS Core::ContactDelegate
	SOURCE_DIR simplecontactdelegate
	DISPLAY_NAME "ContactDelegate"
	DESCRIPTION "Just simple"
)
