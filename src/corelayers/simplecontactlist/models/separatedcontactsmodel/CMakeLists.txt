qutim_add_plugin(separatedcontactsmodel
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/separatedcontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::SeparatedModel
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Separated contacts model"
	DESCRIPTION "Simple separated contactlist model"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)