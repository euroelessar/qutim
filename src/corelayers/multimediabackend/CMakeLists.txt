find_package( Qt4 COMPONENTS QtMultimedia)

## hack for cmake 2.8.0 that does not support QtMultimedia
if( NOT QT_QTMULTIMEDIA_FOUND )
	FIND_PATH( QT_QTMULTIMEDIA_INCLUDE_DIR QtMultimedia
			   PATHS
			   ${QT_HEADERS_DIR}/QtMultimedia
			   ${QT_LIBRARY_DIR}/QtMultimedia.framework/Headers
			   NO_DEFAULT_PATH
	   )
	## Only release library is being searched for.
	FIND_LIBRARY( QT_QTMULTIMEDIA_LIBRARY
				  NAMES QtMultimedia${QT_LIBINFIX} QtMultimedia${QT_LIBINFIX}4
				  PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH
	   )
	if( QT_QTMULTIMEDIA_INCLUDE_DIR AND QT_QTMULTIMEDIA_LIBRARY )
		set( QT_QTMULTIMEDIA_FOUND 1 )
	endif( QT_QTMULTIMEDIA_INCLUDE_DIR AND QT_QTMULTIMEDIA_LIBRARY )
endif( NOT QT_QTMULTIMEDIA_FOUND )
## hack end

if( ${QT_QTMULTIMEDIA_FOUND} )
	include_directories( ${QT_QTMULTIMEDIA_INCLUDE_DIR} )
else( ${QT_QTMULTIMEDIA_FOUND} )
	set( QT_QTMULTIMEDIA_LIBRARY "" )
endif( ${QT_QTMULTIMEDIA_FOUND} )


qutim_add_plugin( multimediabackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/multimediasoundbackend.h
	EXTENSION_CLASS Core::MultimediaSoundBackend
	LINK_LIBRARIES ${QT_QTMULTIMEDIA_LIBRARY}
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mulimedia engine"
	DESCRIPTION "Sound engine based on QtMultimedia"
)
