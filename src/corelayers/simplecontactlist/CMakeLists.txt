include_directories(.)

qutim_add_plugin(contactlist
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

qutim_add_plugin(treecontactsmodel
	EXTENSION
	EXTENSION_HEADER treecontactsmodel/simplecontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::TreeModel
	SOURCE_DIR treecontactsmodel
	DISPLAY_NAME "Tree contacts model"
	DESCRIPTION "Simple contactlist model"
	DEPENDS contactlist
	LINK_LIBRARIES contactlist
)

qutim_add_plugin(plaincontactsmodel
	EXTENSION
	EXTENSION_HEADER plaincontactsmodel/plaincontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::PlainModel
	SOURCE_DIR plaincontactsmodel
	DISPLAY_NAME "Plain contacts model"
	DESCRIPTION "Simple plain contactlist model"
	DEPENDS contactlist
	LINK_LIBRARIES contactlist
)

qutim_add_plugin(separatedcontactsmodel
	EXTENSION
	EXTENSION_HEADER separatedcontactsmodel/separatedcontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::SeparatedModel
	SOURCE_DIR separatedcontactsmodel
	DISPLAY_NAME "Separated contacts model"
	DESCRIPTION "Simple separated contactlist model"
	DEPENDS contactlist
	LINK_LIBRARIES contactlist
)

set(DEVEL_HEADERS
	contactlist/abstractcontactlist.h
	contactlist/abstractcontactmodel.h
	contactlist/simplecontactlistview.h
	contactlist/simplestatusdialog.h
	contactlist/simplecontactlist.h
)
qutim_generate_includes("qutim/simplecontactlist" ${DEVEL_HEADERS})


add_subdirectory(widgets)
