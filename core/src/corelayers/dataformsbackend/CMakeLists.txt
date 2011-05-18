qutim_add_plugin( dataformsbackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/dataformsbackend.h
	EXTENSION_CLASS Core::DefaultDataFormsBackend
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Data forms backend"
	DESCRIPTION "Default qutIM implementation of the data forms"
)
