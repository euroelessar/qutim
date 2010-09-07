macro_ensure_version("4.7.0" "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" QT_HAS_4_7 )
IF(NOT QT_HAS_4_7)
	message("Can't build qmlpopups" )
	return()
ENDIF()

FIND_PACKAGE( Qt4 COMPONENTS QtDeclarative)

qutim_add_plugin( qmlpopups
	DISPLAY_NAME "QML Popups"
	DESCRIPTION "Modern popups, based on power of Qt Quick"
	LINK_LIBRARIES ${QT_QTDECLARATIVE_LIBRARY}
)

QUTIM_ADD_ARTWORK(qmlpopups)
