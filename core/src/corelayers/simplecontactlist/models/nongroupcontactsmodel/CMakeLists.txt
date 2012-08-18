qutim_add_plugin(nongroupcontactsmodel
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/nongroupcontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::NonGroupModel
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "NonGroup contacts model"
	DESCRIPTION "Contact list model, which separates contacts to in list/not in list"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
