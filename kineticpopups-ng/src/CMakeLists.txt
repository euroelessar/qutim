if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	qutim_add_plugin(kineticpopupsng
		EXTENSION
		EXTENSION_CLASS KineticPopups::Backend
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/backend.h
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Kinetic popups (New implementation)"
		DESCRIPTION "Growl-like popups, based on power of Qt Quick"
		COMPILE_FLAGS "-DKINETICPOPUPS_LIBRARY"
	)

else()
	message(STATUS "Plugin is under development. Kineticpopups-ng skipped")
endif()
