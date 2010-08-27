find_package( Qt4 COMPONENTS QtDBus )
include_directories( ${QT_QTDBUS_INCLUDE_DIR} )
qutim_add_plugin( awn
	DISPLAY_NAME "AWN"
	DESCRIPTION "Avant window navigator dock integration"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARIES}
)
