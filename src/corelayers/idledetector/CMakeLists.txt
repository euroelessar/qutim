qutim_add_plugin( idledetector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/idle.h
	EXTENSION_CLASS Psi::Idle
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Idle detector"
	DESCRIPTION "Native idle detector, based on system api"
)
