qutim_add_plugin(tabbedchatform
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/tabbedchatform.h
	EXTENSION_CLASS Core::TabbedChatForm
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Tabbed Chat"
	DESCRIPTION "Adium-like chatform implementation for adiumchat"
)
