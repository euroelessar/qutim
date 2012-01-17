CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Oscar )

option( OSCAR_USE_3RDPARTY_HMAC "Use external hmac-sha256 implemntation instead of QCA2" OFF )
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/hmac")
	set(OSCAR_USE_3RDPARTY_HMAC OFF)
endif()

if( OSCAR_USE_3RDPARTY_HMAC )
	set( HMAC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/hmac )
	add_library( hmac STATIC
		${HMAC_PATH}/hmac_sha2.c
		${HMAC_PATH}/hmac_sha2.h
		${HMAC_PATH}/sha2.c
		${HMAC_PATH}/sha2.h
	)
	set_target_properties(hmac PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
	include_directories(${HMAC_PATH})
	set( OSCAR_ADDITIONAL_COMPILE_FLAGS "-DOSCAR_USE_3RDPARTY_HMAC" )
	set( OSCAR_ADDITIONAL_LIBRARIES hmac )
else()
	find_package( QCA2 )
	if( NOT QCA2_FOUND )
		message( "QCA2 not found. Can't build OSCAR protocol" )
		return()
	endif( NOT QCA2_FOUND )
	include_directories( ${QCA2_INCLUDE_DIR} )
	set( OSCAR_ADDITIONAL_COMPILE_FLAGS "-DOSCAR_USE_QCA2" )
	set( OSCAR_ADDITIONAL_LIBRARIES ${QCA2_LIBRARIES} )
endif()

if( QT_QCONFIG MATCHES "openssl" )
	add_definitions(-DOSCAR_SSL_SUPPORT)
endif()

message( "OSCAR_ADDITIONAL_LIBRARIES = ${OSCAR_ADDITIONAL_LIBRARIES}" )

qutim_add_plugin( oscar
	COMPILE_FLAGS "-DLIBOSCAR_LIBRARY ${OSCAR_ADDITIONAL_COMPILE_FLAGS}"
	LINK_LIBRARIES ${OSCAR_ADDITIONAL_LIBRARIES}
	DISPLAY_NAME "Oscar protocol"
	DESCRIPTION "Module-based implementation of Oscar protocol"
	GROUP "Protocols"
	QT_LIBRARIES NETWORK XML
)

qutim_add_extensions_dir(plugins)
