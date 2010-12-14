add_subdirectory(libs)

include_directories(libs/include)

qutim_add_plugin( win7int2
	DISPLAY_NAME "Windows 7 Integration"
	DESCRIPTION "/* TODO */"
	LINK_LIBRARIES w7apiqtwrapper
)
