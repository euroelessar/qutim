if(NOT APPLE)
	message(STATUS "Macintegration plugin will be skipped")
	return()
endif()

qutim_add_extensions_dir(.)

#reinder, please update this paths
#INSTALL(DIRECTORY share/config
#	DESTINATION share/qutim
#		COMPONENT Core
#)

qutim_add_artwork_dir(share)
