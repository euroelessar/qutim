list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
find_package( LibOTR )

if( NOT LIBOTR_FOUND )
	message( "libotr-3.2.0 not found. Can't build Off-the-Record messaging module" )
	return()
endif( NOT LIBOTR_FOUND )

qutim_add_plugin( offtherecord
	DISPLAY_NAME "Off-the-Record Messaging"
	DESCRIPTION "Plugin provides encryption of messages by OTR"
	LINK_LIBRARIES ${LIBOTR_LIBRARY}
	INCLUDE_DIRS ${LIBOTR_INCLUDE_DIR}
)
