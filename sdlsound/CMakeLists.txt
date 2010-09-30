find_package( SDL_mixer )

if( NOT SDLMIXER_FOUND )
	message( "SDL mixer not found. Can't build sdlsound" )
	return()
endif()

qutim_add_plugin( sdlsound
	DISPLAY_NAME "SDL sound engine"
	DESCRIPTION "Sound engine based on Simple DirectMedia Layer"
	LINK_LIBRARIES ${SDLMIXER_LIBRARY}
	INCLUDE_DIRS ${SDLMIXER_INCLUDE_DIR}
)

