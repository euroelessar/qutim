macro_ensure_version("4.7.0" "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" QT_HAS_4_7 )
IF(NOT QT_HAS_4_7)
	message("Can't build qml popupwidget" )
	return()
ENDIF()

FIND_PACKAGE( Qt4 COMPONENTS QtDeclarative)

qutim_add_plugin( qmlpopupwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/qmlpopup.h
	EXTENSION_CLASS Core::KineticPopups::PopupWidget
	DISPLAY_NAME "QDeclarative Popup Widget"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Modern popups, based on power of Qt Quick"
	LINK_LIBRARIES ${QT_QTDECLARATIVE_LIBRARY} kineticpopups
)

QUTIM_ADD_ARTWORK(qmlpopups)
