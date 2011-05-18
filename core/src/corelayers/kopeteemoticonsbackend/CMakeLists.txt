qutim_add_plugin( kopeteemoticonsbackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/kopeteemoticonsbackend.h
	EXTENSION_CLASS KopeteEmoticonsBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "qutIM and Kopete emoticons"
	DESCRIPTION "Default qutIM emoticons backend"
)
