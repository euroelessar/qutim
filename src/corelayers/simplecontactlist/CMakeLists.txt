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
	SOURCE_DIR simplecontactdelegate
	DISPLAY_NAME "ContactDelegate"
	DESCRIPTION "Just simple"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)

set(DEVEL_HEADERS
	contactlist/abstractcontactlist.h
	contactlist/abstractcontactmodel.h
	contactlist/simplecontactlistview.h
	contactlist/simplestatusdialog.h
	contactlist/simplecontactlist.h
	contactlist/simplecontactlistsettings.h
)
qutim_generate_includes("qutim/simplecontactlist" ${DEVEL_HEADERS})


add_subdirectory(widgets)
add_subdirectory(models)
