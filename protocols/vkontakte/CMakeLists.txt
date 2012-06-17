set(QT_USE_QTNETWORK true)

add_subdirectory(lib)

qutim_add_plugin(vkontakte
	DISPLAY_NAME "vkontakte"
	DESCRIPTION "New implementation of vk.com protocol"
	GROUP "Protocols"
	INCLUDE_DIRS lib/src/api
	LINK_LIBRARIES vk
	QT_LIBRARIES webkit
)
