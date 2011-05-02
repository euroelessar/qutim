qutim_add_plugin(symbianvibra
	GROUP "Symbian integration Plugin"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/symbianvibration.h
	EXTENSION_CLASS Symbian::Vibration
	LINK_LIBRARIES "-l:hwrmvibraclient.dso"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Symbian Vibration Support"
	DESCRIPTION "Notifies about different events by vibration of your mobile device"
)
