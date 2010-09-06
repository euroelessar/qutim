find_package( Phonon )

IF (NOT DEFINED PHONON_FOUND)
	if( PHONON_INCLUDE_DIR AND PHONON_LIBRARY )
		set( PHONON_LIBRARIES "${PHONON_LIBRARY}" )
		set( PHONON_INCLUDES "${PHONON_INCLUDE_DIR}" )
	elseif( QT_PHONON_INCLUDE_DIR AND PHONON_LIBRARY )
		set( PHONON_LIBRARIES "${PHONON_LIBRARY}" )
		set( PHONON_INCLUDES "${QT_PHONON_INCLUDE_DIR}" )
	else( PHONON_INCLUDE_DIR AND PHONON_LIBRARY )
		message( "PHONON not found. Can't build phononsound" )
		return()
	endif()
ENDIF()

qutim_add_plugin( phononsound
	DISPLAY_NAME "Phonon sound engine"
	DESCRIPTION "Sound engine based on KDE sound engine,"
	LINK_LIBRARIES ${PHONON_LIBRARIES}
	INCLUDE_DIRS ${PHONON_INCLUDES}
)

