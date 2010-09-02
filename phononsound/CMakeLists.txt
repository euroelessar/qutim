find_package( Phonon REQUIRED )

IF (NOT DEFINED PHONON_FOUND)
	SET (PHONONSOUND 0)
ENDIF()

qutim_add_plugin( phononsound
	DISPLAY_NAME "Phonon sound engine"
	DESCRIPTION "Sound engine based on KDE sound engine,"
	LINK_LIBRARIES ${PHONON_LIBS_LIBRARY}
	INCLUDE_DIRS ${PHONON_INCLUDES}
)

