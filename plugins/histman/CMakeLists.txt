SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTSQL TRUE )
qutim_add_plugin( histman
	DISPLAY_NAME "HistMan"
	DESCRIPTION "Plugin makes possible to import history to qutIM from different messengers,"
        LINK_LIBRARIES ${QT_QTSQL_LIBRARY} ${QT_QTXML_LIBRARY}
        INCLUDE_DIRS ${QT_QTSQL_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
)
