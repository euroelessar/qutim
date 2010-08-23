qutim_add_plugin( multimediabackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/multimediasoundbackend.h
	EXTENSION_CLASS Core::MultimediaSoundBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mulimedia engine"
	DESCRIPTION "Sound engine based on QtMultimedia"
)
