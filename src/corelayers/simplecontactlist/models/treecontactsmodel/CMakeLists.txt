qutim_add_plugin(treecontactsmodel
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplecontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::TreeModel
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Tree contacts model"
	DESCRIPTION "Simple contactlist model"
	DEPENDS contactlist
	LINK_LIBRARIES contactlist
)