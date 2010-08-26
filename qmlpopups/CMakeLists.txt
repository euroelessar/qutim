SET( QT_USE_QTSCRIPT TRUE )
SET( QT_USE_QTDECLARATIVE TRUE )
find_package( Qt4 REQUIRED )

qutim_add_plugin( qmlpopups
	DISPLAY_NAME "QML Popups"
	DESCRIPTION "Modern popups, based on power of Qt Quick"
)
