# This option is defined at qutim_add_plugin macros,
# but I have to know user's decision before looking for Jreen
option(JABBER "Enable Jabber plugin" ${QUTIM_ENABLE_ALL_PLUGINS})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
if( JABBER )
	option( SYSTEM_JREEN "Use system Jreen" off )
	if( SYSTEM_JREEN )
		find_package( PkgConfig )
		pkg_check_modules( JREEN REQUIRED libjreen )
		link_directories( ${JREEN_LIBRARY_DIRS} )
	else()
		set( JREEN_FORCE_QT4 on CACHE BOOL "" )
		add_subdirectory( jreen )
		list( APPEND INCLUDE_DIRS "${JREEN_INCLUDE_DIR}" )
	endif()
endif()

find_package(QtMobility)
if( QT_MOBILITY_FOUND AND QT_MOBILITY_MULTIMEDIAKIT_FOUND )
	list( APPEND INCLUDE_DIRS ${QT_MOBILITY_MULTIMEDIAKIT_INCLUDE_DIR} ${QT_MOBILITY_INCLUDE_DIR} ${QT_MOBILITY_PARENT_INCLUDE_DIR} )
	list( APPEND JABBER_LINK_LIBRARIES ${QT_MOBILITY_MULTIMEDIAKIT_LIBRARY} )
	add_definitions( "-DJABBER_HAVE_MULTIMEDIA=1" )
else()
	message("Module MultimediaKit not found. Jingle support is disabled.")
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
	LINK_LIBRARIES ${JREEN_LIBRARIES} ${QCA2_LIBRARIES} ${JABBER_LINK_LIBRARIES}
	INCLUDE_DIRS ${JREEN_INCLUDE_DIRS} ${QCA2_INCLUDE_DIR} ${INCLUDE_DIRS}
)
