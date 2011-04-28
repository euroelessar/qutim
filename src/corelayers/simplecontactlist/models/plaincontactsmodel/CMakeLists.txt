qutim_add_plugin(plaincontactsmodel
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/plaincontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::PlainModel
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Plain contacts model"
	DESCRIPTION "Simple plain contactlist model"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
