# This option is defined at qutim_add_plugin macros,
# but I have to know user's decision before looking for Jreen
option(JABBER "Enable Jabber plugin" ${QUTIM_ENABLE_ALL_PLUGINS})
if( JABBER )
	option( SYSTEM_JREEN "Use system Jreen" off )
	if( SYSTEM_JREEN )
		find_package( PkgConfig )
		pkg_check_modules( JREEN REQUIRED libjreen )
	else()
		ADD_SUBDIRECTORY(jreen)
		LIST(APPEND INCLUDE_DIRS "${JREEN_INCLUDE_DIRS}")
	endif()
endif()

find_package(QCA2)
if( NOT QCA2_FOUND )
	message(STATUS "Could not find QCA2, skipping Jabber plugin")
	return()
endif()

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" )

qutim_add_plugin( jabber
	DISPLAY_NAME "Jabber protocol"
	DESCRIPTION "Implementation of jabber protocol based on jreen library"
	GROUP "Protocols"
	LINK_LIBRARIES ${JREEN_LIBRARIES} ${QCA2_LIBRARIES}
	INCLUDE_DIRS ${JREEN_INCLUDE_DIRS} ${QCA2_INCLUDE_DIR}
)
