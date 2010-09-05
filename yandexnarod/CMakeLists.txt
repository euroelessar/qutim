find_package(Qt4 COMPONENTS QtNetwork QtXml QtXmlPatterns QtWebkit)

qutim_add_plugin( yandexnarod
	DISPLAY_NAME "YandexNarod"
	DESCRIPTION "Send files via Yandex.Narod filehosting service"
)
