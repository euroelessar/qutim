include_directories(adiumchat)

qutim_add_plugin(symbianchatform
	EXTENSION
	EXTENSION_HEADER stackedchatform.h
	EXTENSION_CLASS Core::AdiumChat::Symbian::StackedChatForm
	DISPLAY_NAME "Symbian"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Stacked chatform for S60 devices"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat
)
