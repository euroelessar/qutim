macro_ensure_version("4.7.0" "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" QT_HAS_4_7 )
IF(NOT QT_HAS_4_7)
	message("Can't build connectionmanager" )
	return()
ENDIF()
find_package(Qt4 COMPONENTS QtNetwork )
qutim_add_plugin( connectionmanager
	DISPLAY_NAME "ConnectionManager"
	DESCRIPTION ""
)
