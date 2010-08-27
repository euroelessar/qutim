find_package( Qt4 COMPONENTS QtDBus )
include_directories( ${QT_QTDBUS_INCLUDE_DIR} )
qutim_add_plugin( dbusapi
	DISPLAY_NAME "DBus API"
	DESCRIPTION "Added ability to control qutIM by DBus"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARIES}
)
