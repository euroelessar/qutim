qutim_add_plugin(plaincontactsmodel
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/plaincontactlistmodel.h
	EXTENSION_CLASS Core::SimpleContactList::PlainModel
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Plain list contacts model"
	DESCRIPTION "Contact list model, which shows only contacts"
	DEPENDS qutim-simplecontactlist
	LINK_LIBRARIES qutim-simplecontactlist
)
