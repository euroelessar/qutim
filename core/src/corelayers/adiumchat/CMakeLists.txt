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
	chatlayer/chatforms/abstractchatform.h
	chatlayer/chatforms/abstractchatwidget.h
	chatlayer/chatedit.h
	chatlayer/chatlayer_global.h
	chatlayer/chatlayerimpl.h
	chatlayer/chatlayerplugin.h
	chatlayer/chatsessionimpl.h
	chatlayer/chatsessionmodel.h
	chatlayer/chatviewfactory.h
	chatlayer/conferencecontactsview.h
	chatlayer/conferencetabcompletion.h
	chatlayer/sessionlistwidget.h

)
install(FILES ${DEVEL_HEADERS}
	DESTINATION include/qutim/adiumchat
	COMPONENT CoreDevel
)
qutim_generate_includes("qutim/adiumchat" ${DEVEL_HEADERS})
