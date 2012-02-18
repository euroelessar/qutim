set(QT_USE_QTNETWORK true)
set(QT_USE_QTSCRIPT true)
set(QT_USE_QTWEBKIT true)

qutim_add_plugin(vkontakte
	COMPILE_FLAGS "-DVKONTAKTE_LIBRARY"
	DISPLAY_NAME "VKontakte"
	DESCRIPTION "Simple implementation of vkontakte"
	GROUP "Protocols"
	QT_LIBRARIES WEBKIT
)

qutim_add_extensions_dir(plugins)
