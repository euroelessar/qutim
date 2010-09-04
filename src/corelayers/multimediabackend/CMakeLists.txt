find_package( Qt4 COMPONENTS QtMultimedia)

if( NOT QT_QTMULTIMEDIA_FOUND )
	message( "QtMultimedia not found. Can't build multimediabackend" )
	return()
endif( NOT QT_QTMULTIMEDIA_FOUND )

qutim_add_plugin( multimediabackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/multimediasoundbackend.h
	EXTENSION_CLASS Core::MultimediaSoundBackend
	LINK_LIBRARIES ${QT_QTMULTIMEDIA_LIBRARY}
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mulimedia engine"
	DESCRIPTION "Sound engine based on QtMultimedia"
	INCLUDE_DIRS ${QT_QTMULTIMEDIA_INCLUDE_DIR}
)
