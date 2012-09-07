if(NOT APPLE)
	message(STATUS "Macintegration plugin will be skipped")
	return()
endif()

add_subdirectory(src)
qutim_add_artwork_dir(share)
