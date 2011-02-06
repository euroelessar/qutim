include_directories(.
	chatlayer
	)

qutim_add_plugin(adiumchat
	SUBPLUGIN
	SOURCE_DIR chatlayer
	DISPLAY_NAME "Webkit chat layer"
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	COMPILE_FLAGS "-DADIUMCHAT_LIBRARY"
)

add_subdirectory(chatforms)
add_subdirectory(chatfactories)