CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Oscar )

set( HMAC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/hmac )
add_library( hmac STATIC
	${HMAC_PATH}/hmac_sha2.c
	${HMAC_PATH}/hmac_sha2.h
	${HMAC_PATH}/sha2.c
	${HMAC_PATH}/sha2.h
)
set_target_properties(hmac PROPERTIES COMPILE_FLAGS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})
include_directories(${HMAC_PATH})

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTXML TRUE )
qutim_add_plugin( oscar
	COMPILE_FLAGS "-DLIBOSCAR_LIBRARY"
	LINK_LIBRARIES hmac
	DISPLAY_NAME "Oscar protocol"
	DESCRIPTION "Module-based implementation of Oscar protocol"
	GROUP "Protocols"
)
IF( QT_QCONFIG MATCHES "openssl" )
	add_definitions(-DOSCAR_SSL_SUPPORT)
ENDIF( QT_QCONFIG MATCHES "openssl" )

qutim_add_extensions_dir(plugins)
