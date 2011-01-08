CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0011 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT ( Jabber )
SET ( QT_MIN_VERSION "4.6.0" )

SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/" )

ADD_SUBDIRECTORY(jreen)
LIST(APPEND INCLUDE_DIRS "${JREEN_INCLUDE_DIR}")

SET( QT_USE_QTNETWORK TRUE )

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on jreen library"
	GROUP "Protocols"
	LINK_LIBRARIES jreen
	INCLUDE_DIRS "${INCLUDE_DIRS}"
)
