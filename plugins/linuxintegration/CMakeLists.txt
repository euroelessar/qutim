if(NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(STATUS "Linux integration will be skipped")
	return()
endif()

qutim_add_plugin(linuxintegration
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/linuxintegration.h
	EXTENSION_CLASS LinuxIntegration
	DISPLAY_NAME "Linux integration"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Linux integration"
)
