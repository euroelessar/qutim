qutim_add_plugin(urihandler_jabber
	SUBPLUGIN
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "UriHandler XMPP subplugin"
	DESCRIPTION "UriHandler XMPP subplugin."
	LINK_LIBRARIES urihandler
)
