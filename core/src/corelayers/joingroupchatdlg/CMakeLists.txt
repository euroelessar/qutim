qutim_add_plugin( joingroupchatdlg
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/joingroupchatmodule.h
	EXTENSION_CLASS Core::JoinGroupChatModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Join GroupChat dialog"
	DESCRIPTION "Simple groupchat join dialog"
)
