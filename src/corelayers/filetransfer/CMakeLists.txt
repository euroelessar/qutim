qutim_add_plugin( filetransfer
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/simplefiletransfer.h
	EXTENSION_CLASS Core::SimpleFileTransfer
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "File transfer manager"
	DESCRIPTION "Default qutIM file transfer manager"
)
