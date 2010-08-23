qutim_add_plugin( qticons
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/iconsloaderimpl.h
	EXTENSION_CLASS Core::IconLoaderImpl
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Xdg Icon Loader"
	DESCRIPTION "Default qutIM icon loader implementation. Based on Q-XDG library"
)
