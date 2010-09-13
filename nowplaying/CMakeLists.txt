include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( QT_USE_QTDBUS TRUE )

qutim_add_plugin( "nowplaying"
	DISPLAY_NAME "NowPlaying"
	DESCRIPTION "Now playing plugin"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
