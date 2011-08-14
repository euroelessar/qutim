CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( VKONTAKTE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTSCRIPT TRUE )
SET( QT_USE_QTWEBKIT true)

qutim_add_plugin( vkontakte
	COMPILE_FLAGS "-DVKONTAKTE_LIBRARY"
	DISPLAY_NAME "VKontakte"
	DESCRIPTION "Simple implementation of vkontakte"
	GROUP "Protocols"
)

qutim_add_extensions_dir(plugins)
