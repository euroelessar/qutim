set(QT_USE_QTNETWORK true)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
	pkg_check_modules(VKIT vkit)
endif()
if(NOT VKIT_FOUND)
	if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
		set(VKIT_IMPORTS_DIR bin)
	endif()
	set(VKIT_WITH_OAUTH OFF)
	set(VKIT_WITH_QMLAPI OFF)
	add_subdirectory(lib)
	file(GLOB_RECURSE DEVEL_HEADERS "lib/src/api/*[^p].h")
	qutim_generate_includes("vk" ${DEVEL_HEADERS})
	message(STATUS "Using internal copy of vkit")
	set(VKIT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
	set(VKIT_LIBRARIES vkit CACHE INTERNAL "")
endif()

qutim_add_plugin(vkontakte
	DISPLAY_NAME "vkontakte"
	DESCRIPTION "New implementation of vk.com protocol"
	GROUP "Protocols"
	INCLUDE_DIRS ${VKIT_INCLUDE_DIRS}
	LINK_LIBRARIES ${VKIT_LIBRARIES}
	QT_LIBRARIES webkit declarative
)

qutim_add_artwork(share/vphotoalbum)
