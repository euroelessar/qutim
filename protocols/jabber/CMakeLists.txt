if(${JABBER} EQUAL 0 OR ${JABBER} STREQUAL "OFF")
else()
	option( SYSTEM_JREEN "Use system Jreen" off )
	if( SYSTEM_JREEN )
		find_package( PkgConfig )
		pkg_check_modules( JREEN REQUIRED libjreen )
	else()
		ADD_SUBDIRECTORY(jreen)
	endif()
endif()

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on jreen library"
	GROUP "Protocols"
	LINK_LIBRARIES ${JREEN_LIBRARIES}
	INCLUDE_DIRS ${JREEN_INCLUDE_DIRS}
)
