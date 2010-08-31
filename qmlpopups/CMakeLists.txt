SET(QT_MIN_VERSION "4.7.0")
find_package( Qt4 COMPONENTS QtDeclarative)

qutim_add_plugin( qmlpopups
	DISPLAY_NAME "QML Popups"
	DESCRIPTION "Modern popups, based on power of Qt Quick"
	LINK_LIBRARIES qmlpopops ${QT_QTDECLARATIVE_LIBRARY}
)
