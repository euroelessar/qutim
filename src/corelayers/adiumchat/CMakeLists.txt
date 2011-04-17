include_directories(.
	chatlayer
	)

qutim_add_plugin(adiumchat
	SUBPLUGIN
	SOURCE_DIR chatlayer
	DISPLAY_NAME "Webkit chat layer"
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	COMPILE_FLAGS "-DADIUMCHAT_LIBRARY"
)

add_subdirectory(chatforms)
add_subdirectory(chatfactories)

set(DEVEL_HEADERS
	chatlayer/chatviewfactory.h
	chatlayer/chatsessionimpl.h
	chatlayer/chatforms/abstractchatwidget.h
	chatlayer/chatforms/abstractchatform.h
	chatlayer/sessionlistwidget.h
	chatlayer/conferencecontactsview.h
	chatlayer/chatlayerimpl.h
	chatlayer/chatedit.h
	chatlayer/chatlayer_global.h

)
qutim_generate_includes("qutim/adiumchat" ${DEVEL_HEADERS})
