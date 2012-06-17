set(QT_USE_QTNETWORK true)

add_subdirectory(lib)

file(GLOB_RECURSE DEVEL_HEADERS "lib/src/api/*[^p].h")
qutim_generate_includes("vk" ${DEVEL_HEADERS})

qutim_add_plugin(vkontakte
	DISPLAY_NAME "vkontakte"
	DESCRIPTION "New implementation of vk.com protocol"
	GROUP "Protocols"
	INCLUDE_DIRS
	LINK_LIBRARIES vk
	QT_LIBRARIES webkit
)
