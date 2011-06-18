qutim_add_plugin( joingroupchatdlg
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/joingroupchatmodule.h
	EXTENSION_CLASS Core::JoinGroupChatModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mobile conference join dialog"
	DESCRIPTION "Conference join dialog adopted for touch devices with small screen"
)
