if(NOT MAEMO5)
	message(STATUS "Maemo 5 integration will be skipped")
	return()
endif()

FILE( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

qutim_add_extensions_dir(.)
