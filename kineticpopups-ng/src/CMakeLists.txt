if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	find_package(Qt4 COMPONENTS QtDeclarative)

	qutim_add_plugin(kineticpopupsng
		EXTENSION
		EXTENSION_CLASS KineticPopups::Backend
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/backend.h
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Kinetic popups (New implementation)"
		DESCRIPTION "Growl-like popups, based on power of Qt Quick"
		COMPILE_FLAGS "-DKINETICPOPUPS_LIBRARY"
		LINK_LIBRARIES ${QT_QTDECLARATIVE_LIBRARY}
	)

	QUTIM_ADD_ARTWORK(quickpopup)

else()
	message(STATUS "Plugin is under development. Kineticpopups-ng skipped")
endif()
