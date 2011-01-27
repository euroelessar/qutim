SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
find_package(WinThings)

add_subdirectory(libs)

if(WINTHINGS_APILAYER_LIBRARY)
	qutim_add_plugin(
		win7int2
		DISPLAY_NAME   "Windows 7 Integration" # now it is Win7int, but in future…
		DESCRIPTION    "Adds count of unread messages as an icon to taskbar button of qutim, along with some commands list."
		GROUP          "Plugins"
		LINK_LIBRARIES wapiqtwrapper
		INCLUDE_DIRS   "libs/include"
	)
	if(MSVC)
		set_target_properties(win7int2 PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-")
	endif()
else()
	message(STATUS "'Windows Integration' plugin will be skipped.")
endif()
