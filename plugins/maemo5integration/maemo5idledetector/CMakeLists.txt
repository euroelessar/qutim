FIND_PACKAGE( Qt4 COMPONENTS QtDBus)
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "Cannot build Maemo5IdleDetector")
	return()
endif()

qutim_add_plugin( maemo5idledetector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/maemo5idle.h
	EXTENSION_CLASS Maemo5Idle
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Maemo 5 Idle detector"
	DESCRIPTION "Native Maemo 5 idle detector"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARY}
	INCLUDE_DIRS  ${QT_QTDBUS_INCLUDE_DIR}
)
