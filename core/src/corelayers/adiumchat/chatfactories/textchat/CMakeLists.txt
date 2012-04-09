qutim_add_plugin(textchat
	GROUP "ChatFactory"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/textviewfactory.h
	EXTENSION_CLASS "Core::AdiumChat::TextViewFactory, Core::AdiumChat::ChatViewFactory"
	DISPLAY_NAME "Text view"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Fast and light chat view"
	DEPENDS adiumchat
)
