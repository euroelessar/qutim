qutim_add_plugin( classicchatform
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/classicchatform.h
	EXTENSION_CLASS Core::ClassicChatForm
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Classic"
	DESCRIPTION "Default qutIM chatform implementation for adiumchat"
)
