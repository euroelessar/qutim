CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Oscar )

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTXML TRUE )
qutim_add_plugin( oscar
	COMPILE_FLAGS "-DLIBOSCAR_LIBRARY"
	DISPLAY_NAME "Oscar protocol"
	DESCRIPTION "Module-based implementation of Oscar protocol"
	GROUP "Protocols"
)
IF( QT_QCONFIG MATCHES "openssl" )
	add_definitions(-DOSCAR_SSL_SUPPORT)
ENDIF( QT_QCONFIG MATCHES "openssl" )

qutim_add_extensions_dir(plugins)
