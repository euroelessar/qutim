SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTXMLPATTERNS TRUE )
SET( QT_USE_QTWEBKIT TRUE )

qutim_add_plugin( yandexnarod
	DISPLAY_NAME "YandexNarod"
	DESCRIPTION "Send files via Yandex.Narod filehosting service"
)
