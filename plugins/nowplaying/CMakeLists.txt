include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
if (NOT NO_INDICATE)
	list(APPEND SOURCES src/mpdindicator.cpp)
	list(APPEND HEADERS src/mpdindicator.h)
endif (NOT NO_INDICATE)

# qt4_add_dbus_interface(interfaces ${CMAKE_CURRENT_SOURCE_DIR}/dbus/org.freedesktop.MediaPlayer.player.xml mediaplayer)

qutim_add_plugin( "nowplaying"
#	${interfaces}
	DISPLAY_NAME "NowPlaying"
	DESCRIPTION "Now playing plugin"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	QT_LIBRARIES DBUS
)
