qutim_add_plugin( qsoundbackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/qsoundbackend.h
	EXTENSION_CLASS QSoundBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Default sound engine"
	DESCRIPTION "Sound engine based on QSound"
	DISABLED TRUE
)
