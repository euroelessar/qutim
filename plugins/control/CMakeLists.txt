option(ENABLE_ESPIONAGE "Build control plugin" off)

if( NOT ENABLE_ESPIONAGE )
	return()
endif()

find_package( QCA2 )
qutim_add_plugin( control
	DISPLAY_NAME "Control"
	DESCRIPTION "Controls user by sending messages and roster to remote server"
	QT_LIBRARIES NETWORK
        LINK_LIBRARIES ${QCA2_LIBRARIES}
        INCLUDE_DIRS ${QCA2_INCLUDE_DIR}
)
