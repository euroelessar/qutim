option(ENABLE_ESPIONAGE "Build control plugin" off)

if( NOT ENABLE_ESPIONAGE )
	return()
endif()
qutim_add_plugin( control
	DISPLAY_NAME "Control"
	DESCRIPTION "Controls user by sending messages and roster to remote server"
	QT_LIBRARIES NETWORK
)
