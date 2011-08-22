find_package(Qt4 COMPONENTS QtNetwork QtXml)

qutim_add_plugin( yandexnarod
	DISPLAY_NAME "YandexNarod"
	DESCRIPTION "Send files via Yandex.Narod filehosting service"
	LINK_LIBRARIES ${QT_QTXML_LIBRARY}
	INCLUDE_DIRS ${QT_QTXML_INCLUDE_DIR}
)
