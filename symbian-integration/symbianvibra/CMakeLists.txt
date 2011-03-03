SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
find_package(HWRMVibra)

IF(HWRMVIBRA_FOUND)
	qutim_add_plugin(symbianvibra
		EXTENSION
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/symbianvibration.h
		EXTENSION_CLASS Symbian::Vibration
		LINK_LIBRARIES ${HWRMVIBRA_LIBRARIES}
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Symbian Vibration Support"
		DESCRIPTION "Notifies about different events by vibration of your mobile device"
	)
ELSE()
	message(STATUS "Symbian Vibration Support will be skipped")
ENDIF()
