find_package( Phonon REQUIRED )

qutim_add_plugin( phononsound
	DISPLAY_NAME "Phonon sound engine"
	DESCRIPTION "Sound engine based on KDE sound engine,"
	LINK_LIBRARIES ${PHONON_LIBS}
)
