qutim_add_plugin( filetransfersettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/filetransfersettings.h
	EXTENSION_CLASS Core::FileTransferSettings,Ureen::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "File transfer settings"
	DESCRIPTION "Default implementation of the file transfer settings widget"
)
