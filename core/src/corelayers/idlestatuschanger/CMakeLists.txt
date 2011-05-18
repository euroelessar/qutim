qutim_add_plugin( idlestatuschanger
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/idlestatuschanger.h
	EXTENSION_CLASS Core::IdleStatusChanger
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "AutoAway"
	DESCRIPTION "Automatically changes status of accounts on inactivity"
)
