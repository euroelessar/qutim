qutim_add_plugin( metacontacts
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/manager.h
	EXTENSION_CLASS Core::MetaContacts::Manager
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple MetaContacts"
        DESCRIPTION "Default qutIM MetaContacts implementation"
)
