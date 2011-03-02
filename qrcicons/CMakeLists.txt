qutim_add_plugin(qrcicons
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/qresourceiconloader.h
	EXTENSION_CLASS Embedded::ResourceIconLoader
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Resource Icon Loader"
	DESCRIPTION "Very simple icon loader from compiled resources for embedded devices"
)

