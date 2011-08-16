FIND_PACKAGE( Qt4 COMPONENTS QtDBus)
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "Cannot build Maemo5Vibra")
	return()
endif()

qutim_add_plugin(maemo5vibra
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/maemo5vibra.h
	EXTENSION_CLASS Maemo5Vibra
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Maemo 5 Vibra Notifications"
	DESCRIPTION "Notification system, based Maemo 5 Vibrator"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARY}
	INCLUDE_DIRS ${QT_QTDBUS_INCLUDE_DIR}
)
