find_package( Phonon REQUIRED )

IF (NOT DEFINED PHONON_FOUND)
	message( "PHONON not found. Can't build phononsound" )
	return()
ENDIF()

qutim_add_plugin( phononsound
	DISPLAY_NAME "Phonon sound engine"
	DESCRIPTION "Sound engine based on KDE sound engine,"
	LINK_LIBRARIES ${PHONON_LIBRARIES}
	INCLUDE_DIRS ${PHONON_INCLUDES}
)

