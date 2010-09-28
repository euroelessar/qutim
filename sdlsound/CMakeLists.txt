find_package( PkgConfig )
pkg_check_modules( SDL_MIXER SDL_mixer>=1.0 )

if( NOT SDL_MIXER_FOUND )
	message( "SDL mixer not found. Can't build sdlsound" )
	return()
endif()

qutim_add_plugin( sdlsound
	DISPLAY_NAME "SDL sound engine"
	DESCRIPTION "Sound engine based on Simple DirectMedia Layer"
	LINK_LIBRARIES ${SDL_MIXER_LIBRARIES}
	INCLUDE_DIRS ${SDL_MIXER_INCLUDE_DIR}
)

