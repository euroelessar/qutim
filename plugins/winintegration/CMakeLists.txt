SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(WinThings)

add_subdirectory(libs)

if(WINTHINGS_APILAYER_LIBRARY)
	if (MSVC)
        set(WININT_CFLAGS "/Zc:wchar_t-")
    endif()
	
	qutim_add_plugin( winint2
		DISPLAY_NAME   "Windows Integration"
		DESCRIPTION    "Adds count of unread messages as an icon to taskbar button of qutim, along with some commands list."
		GROUP "Windows Seven Integration"
		LINK_LIBRARIES wapiqtwrapper ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTNETWORK_LIBRARY}
		INCLUDE_DIRS   "libs/include" ${QT_QTNETWORK_INCLUDE_DIR}
        COMPILE_FLAGS ${WININT_CFLAGS}
	)

	ADD_SUBDIRECTORY(wininthelper)

	FILE(GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/subplugins" "${CMAKE_CURRENT_SOURCE_DIR}/subplugins/*")

	qutim_add_extensions_dir(subplugins)

	IF(QUTIM_INSTALL_ICONS)
		FILE(GLOB WIN_ICONS "../../artwork/icons/tray/win/*.svg")
		INSTALL(FILES ${WIN_ICONS}
			DESTINATION share/icons/qutim-default/scalable/status/
			COMPONENT Core
		)
	ENDIF()

else()
	message(STATUS "'Windows Integration' plugin will be skipped.")
endif()
