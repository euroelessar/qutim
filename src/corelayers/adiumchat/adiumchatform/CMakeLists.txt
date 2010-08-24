qutim_add_plugin( adiumchatform
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/adiumchatform.h
	EXTENSION_CLASS Core::AdiumChatForm
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Adium"
	DESCRIPTION "Adium-like chatform implementation for adiumchat"
)
