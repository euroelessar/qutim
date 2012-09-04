if(APPLE)
	#cmake cannot deploy docktile plugins properly
	return()
endif()
option(SYSTEM_QTDOCKTILE "Force use system qtdocktile instead build-in copy" OFF)

find_package(PkgConfig)
if(SYSTEM_QTDOCKTILE)
	if(PKG_CONFIG_FOUND)
		pkg_check_modules(QT_QTDOCKTILE REQUIRED qtdocktile)
		#another spike. In pkgconfig LIBRARIES macro list only lib names
		set(QT_QTDOCKTILE_LIBRARIES ${QT_QTDOCKTILE_LDFLAGS})
	endif()
endif()
if(NOT QT_QTDOCKTILE_FOUND)
	if(UNIX AND NOT APPLE)
		pkg_check_modules(DBUSMENUQT dbusmenu-qt>=0.8.1)
		if(NOT DBUSMENUQT_FOUND)
			return()
		endif()
	endif()

	if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
		set(QT_QTDOCKTILE_IMPORTS_DIR bin)
		if(NOT APPLE)
			set(QT_QTDOCKTILE_PLUGINS_DIR bin)
		else()
			set(QT_QTDOCKTILE_PLUGINS_DIR ${QUTIM_BUNDLE_NAME}/Contents/PlugIns)
		endif()
	endif()
	add_subdirectory(qtdocktile)
	set(QT_QTDOCKTILE_WITH_QMLAPI OFF CACHE INTERNAL "")
	set(QT_QTDOCKTILE_INSTALL_HEADERS OFF CACHE INTERNAL "")
	message(STATUS "Using internal copy of qtdocktile")
	set(QT_QTDOCKTILE_LIBRARIES qtdocktile CACHE INTERNAL "")
	set(QT_QTDOCKTILE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/qtdocktile/include/QtDockTile CACHE INTERNAL "")

	#evil hack for reindeer deploy script
	if(APPLE)
		get_target_property(DOCK_PLUGIN_LOCATION macdocktile LOCATION)
		set(_pluginDir "${QUTIM_BUNDLE_LOCATION}/Contents/PlugIns/docktile/macdocktile.dylib")
		add_custom_command(TARGET qutim-bundle POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy
			${DOCK_PLUGIN_LOCATION} ${_pluginDir}
		)
	endif()
endif()

qutim_add_plugin("docktile"
	DISPLAY_NAME "DockTile"
	DESCRIPTION "Dock integration"
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src"
	LINK_LIBRARIES	${QT_QTDOCKTILE_LIBRARIES}
	INCLUDE_DIRS ${QT_QTDOCKTILE_INCLUDE_DIRS}
)

