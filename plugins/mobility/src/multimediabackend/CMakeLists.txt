qutim_check_component(MultimediaKit)

qutim_add_plugin(multimediabackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/multimediasoundbackend.h
	EXTENSION_CLASS Core::MultimediaSoundBackend
	LINK_LIBRARIES ${QT_MOBILITY_MULTIMEDIAKIT_LIBRARY}
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mulimedia engine"
	DESCRIPTION "Sound engine based on QtMultimedia"
	INCLUDE_DIRS ${QT_MOBILITY_MULTIMEDIAKIT_INCLUDE_DIR} ${QT_QTMULTIMEDIA_INCLUDE_DIR}
)
