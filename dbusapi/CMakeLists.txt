find_package(Qt4 COMPONENTS QtDBus )

qutim_add_plugin( dbusapi
	DISPLAY_NAME "DBus API"
	DESCRIPTION "Added ability to control qutIM by DBus"
	LINK_LIBRARIES ${QT_QTDBUS_LIBRARIES}
	INCLUDE_DIRS ${QT_QTDBUS_INCLUDE_DIR}
)
