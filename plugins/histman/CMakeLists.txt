SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTSQL TRUE )
qutim_add_plugin( histman
	DISPLAY_NAME "HistMan"
	DESCRIPTION "Plugin makes possible to import history to qutIM from different messengers"
        QT_LIBRARIES SQL XML
)
