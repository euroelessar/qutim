qutim_add_plugin( addcontactdlg
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/addcontact.h
	EXTENSION_CLASS Core::AddContactModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Add contact dialog"
	DESCRIPTION "Simple add contact dialog"
)
