find_package( Qt4 COMPONENTS QtWebkit )

include_directories(.
	chatlayer)

qutim_add_plugin(adiumchat
	SUBPLUGIN
	SOURCE_DIR chatlayer
	DISPLAY_NAME "Webkit chat layer"
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	LINK_LIBRARIES ${QT_QTWEBKIT}
	COMPILE_FLAGS "-DADIUMCHAT_LIBRARY"
)

qutim_add_plugin(tabbedchatform
	EXTENSION
	EXTENSION_HEADER tabbedchatform/tabbedchatform.h
	EXTENSION_CLASS Core::AdiumChat::TabbedChatForm
	DISPLAY_NAME "Classic"
	SOURCE_DIR tabbedchatform
	DESCRIPTION "Default qutIM chatform implementation for adiumchat"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat
)

qutim_add_plugin(stackedchatform
	EXTENSION
	EXTENSION_HEADER stackedchatform/stackedchatform.h
	EXTENSION_CLASS Core::AdiumChat::StackedChatForm
	DISPLAY_NAME "Mobile"
	SOURCE_DIR stackedchatform
	DESCRIPTION "Stacked chatform for mobile devices"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat
)

qutim_add_plugin(webkitchat
	EXTENSION
	EXTENSION_HEADER webkitchat/webkitviewfactory.h
	EXTENSION_CLASS Core::AdiumChat::WebkitViewFactory
	DISPLAY_NAME "Adium"
	SOURCE_DIR webkitchat
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat
)
