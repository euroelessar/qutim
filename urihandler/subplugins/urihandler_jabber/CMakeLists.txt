#if(NOT DEFINED JABBER_PROTO_PATH)
#	set (JABBER_PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../protocols/jabber/" CACHE PATH "QutIM jabber protocol include files.")
#endif()

#message("OLOLOLO! ${JABBER_PROTO_PATH}")

#if (EXISTS ${JABBER_PROTO_PATH})
	qutim_add_plugin(urihandler_jabber
		SUBPLUGIN
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "UriHandler XMPP subplugin"
		DESCRIPTION "UriHandler XMPP subplugin."
		LINK_LIBRARIES urihandler
		INCLUDE_DIRS "${JABBER_PROTO_PATH}/src/protocol/"
	)
#endif()
