if (NOT NO_INDICATE)
	find_package(Qt4 COMPONENTS QtDBus )
	if(NOT QT_QTDBUS_FOUND)
		message(STATUS "Cannot build now playing")
		return()
	endif()
	list(APPEND SOURCES src/mpdindicator.cpp)
	list(APPEND HEADERS src/mpdindicator.h)
endif (NOT NO_INDICATE)

qutim_add_plugin( "nowplaying"
	DISPLAY_NAME "NowPlaying"
	DESCRIPTION "Now playing plugin"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
