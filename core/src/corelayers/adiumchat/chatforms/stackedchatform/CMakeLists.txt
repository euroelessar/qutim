qutim_add_plugin(stackedchatform
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/stackedchatform.h
	EXTENSION_CLASS Core::AdiumChat::StackedChatForm
	DISPLAY_NAME "Mobile"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Stacked chatform for mobile devices"
	DEPENDS qutim-adiumchat
	LINK_LIBRARIES qutim-adiumchat
)
