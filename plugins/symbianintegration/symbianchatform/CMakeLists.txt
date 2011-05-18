qutim_add_plugin(symbianchatform
	GROUP "Symbian integration Plugin"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/stackedchatform.h
	EXTENSION_CLASS Core::AdiumChat::Symbian::StackedChatForm
	DISPLAY_NAME "Symbian"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Stacked chatform for S60 devices"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
)
