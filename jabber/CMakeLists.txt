if(${JABBER} EQUAL 0 OR ${JABBER} STREQUAL "OFF")

else()
	SET ( QT_MIN_VERSION "4.7.0" )
	SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/" )
	ADD_SUBDIRECTORY(jreen)
	LIST(APPEND INCLUDE_DIRS "${JREEN_INCLUDE_DIR}")
	SET( QT_USE_QTNETWORK TRUE )
endif()

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on jreen library"
	GROUP "Protocols"
	LINK_LIBRARIES jreen
	INCLUDE_DIRS "${INCLUDE_DIRS}"
)
