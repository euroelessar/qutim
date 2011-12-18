qutim_add_plugin(webkitchat
	GROUP "ChatFactory"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/webkitviewfactory.h
	EXTENSION_CLASS Core::AdiumChat::WebkitViewFactory
	DISPLAY_NAME "Adium"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat 
	QT_LIBRARIES WEBKIT
)
