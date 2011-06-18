qutim_add_plugin( joinchatdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/joinchatmodule.h
	EXTENSION_CLASS Core::JoinChatModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Simple conference join dialog"
	DESCRIPTION "Conference join dialog adopted for Desktop systems"
)
