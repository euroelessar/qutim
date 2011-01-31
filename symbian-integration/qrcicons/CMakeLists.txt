if(SYMBIAN)
	qutim_add_plugin(qrcicons
		EXTENSION
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/qresourceiconloader.h
		EXTENSION_CLASS Core::ResourceIconLoader
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Resource Icon Loader"
		DESCRIPTION "Very simple icon loader from compiled resources for embedded devices"
	)
else()
	message(STATUS "Qt Resource plugin is temporary available only for symbian platform")
	return()
endif()
