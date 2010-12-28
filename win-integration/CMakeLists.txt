SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package(WinThings)

if(WINTHINGS_FOUND)
	set(USING_PREBUILT_WINTHINGS true)
	set(WININT2_DEP_INCLUDE_DIRS ${WINTHINGS_INCLUDE_DIR})
	set(WININT2_DEP_LIBRARIES    ${WINTHINGS_LIBRARIES})
else()
	set(USING_PREBUILT_WINTHINGS false)
	if((NOT MSVC) AND (NOT EXISTS ${WINTHINGS_APILAYER_LIBRARY}))
		message(STATUS "Windows 7 Integration plugin will be skipped")
		return()
	endif()
	add_subdirectory("libs")
	set(WININT2_DEP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/libs/include/")
	set(WININT2_DEP_LIBRARIES    wapiqtwrapper)
endif()

qutim_add_plugin( win7int2
	DISPLAY_NAME   "Windows 7 Integration" # now it is Win7int, but in future…
	DESCRIPTION    "Adds count of unread messages as an icon to taskbar button of qutim, along with some commands list."
	GROUP          "Plugins"
	LINK_LIBRARIES "${WININT2_DEP_LIBRARIES}"
	INCLUDE_DIRS   "${WININT2_DEP_INCLUDE_DIRS}"
)
