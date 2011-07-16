if(${JABBER} EQUAL 0 OR ${JABBER} STREQUAL "OFF")

else()
	set( QT_MIN_VERSION "4.7.0" )
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
	add_subdirectory( jreen )
	list( APPEND INCLUDE_DIRS "${JREEN_INCLUDE_DIR}" )
	set( QT_USE_QTNETWORK TRUE )

	find_package(QtMobility)
	if(${QT_MOBILITY_FOUND} AND ${QT_MOBILITY_MULTIMEDIAKIT_FOUND})
		list( APPEND INCLUDE_DIRS ${QT_MOBILITY_MULTIMEDIAKIT_INCLUDE_DIR} ${QT_MOBILITY_INCLUDE_DIR} ${QT_MOBILITY_PARENT_INCLUDE_DIR} )
		list( APPEND JABBER_LINK_LIBRARIES ${QT_MOBILITY_MULTIMEDIAKIT_LIBRARY} )
		add_definitions( "-DJABBER_HAVE_MULTIMEDIA=1" )
	else()
		message("Module MultimediaKit not found. Jingle support is disabled.")
	endif()
endif()

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on jreen library"
	GROUP "Protocols"
	LINK_LIBRARIES jreen ${JABBER_LINK_LIBRARIES}
	INCLUDE_DIRS ${INCLUDE_DIRS}
)
