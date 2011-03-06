qutim_add_plugin(winint2_win7taskbar
	SUBPLUGIN
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Windows Integration: Taskbar"
	DESCRIPTION "Adds count of unread messages as an icon to taskbar button of qutIM."
	LINK_LIBRARIES winint2 wapiqtwrapper
	INCLUDE_DIRS   "../../libs/include"
)
