find_package( Qt4 COMPONENTS QtDBus )
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "Cannot build AWN dock integration")
	return()
endif()

include_directories( ${QT_QTDBUS_INCLUDE_DIR} )
qutim_add_plugin( awn
	DISPLAY_NAME "AWN"
	DESCRIPTION "Avant window navigator dock integration"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARIES}
)
