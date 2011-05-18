find_package(Qt4 COMPONENTS QtNetwork )

qutim_add_plugin( connectionmanager
	DISPLAY_NAME "ConnectionManager"
	DESCRIPTION "Used to monitor the availability of network."
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/connectionmanager.h
	EXTENSION_CLASS ConnectionManager::ConnectionManager
)
