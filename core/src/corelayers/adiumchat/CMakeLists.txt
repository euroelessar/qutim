include_directories(.
	chatlayer
	)

qutim_add_plugin(adiumchat
	SUBPLUGIN
	SOURCE_DIR chatlayer
	DISPLAY_NAME "Webkit chat layer"
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	DEPENDS qutim-adiumchat
	LINK_LIBRARIES qutim-adiumchat
)

add_subdirectory(lib)
add_subdirectory(chatforms)
add_subdirectory(chatfactories)

set(DEVEL_HEADERS
	lib/abstractchatform.h
	lib/abstractchatwidget.h
	lib/chatedit.h
	lib/chatlayer_global.h
	lib/chatlayerimpl.h
	lib/chatsessionimpl.h
	lib/chatsessionmodel.h
	lib/chatviewfactory.h
	lib/conferencecontactsview.h
	lib/conferencetabcompletion.h
	lib/sessionlistwidget.h

)
install(FILES ${DEVEL_HEADERS}
	DESTINATION include/qutim/adiumchat
	COMPONENT CoreDevel
)
qutim_generate_includes("qutim/adiumchat" ${DEVEL_HEADERS})
