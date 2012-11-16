qutim_add_plugin(tabbedchatform
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/tabbedchatform.h
	EXTENSION_CLASS Core::AdiumChat::TabbedChatForm
	DISPLAY_NAME "Classic"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Default qutIM chatform implementation for adiumchat"
	DEPENDS qutim-adiumchat
	LINK_LIBRARIES qutim-adiumchat
)
