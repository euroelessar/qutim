SET ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

if(MSVC)
	add_subdirectory(libs)
else()
	#Viv
	find_package(ApiLayer)
	if(NOT APILAYER_FOUND)
		message(STATUS "Windows 7 Integration plugin will be skipped")
		return()
	endif()
	list(APPEND w7apiqtwrapper ${APILAYER_LIBRARIES})
	list(APPEND INCLUDE_DIRS ${APILAYER_INCLUDE_DIR})
endif()

include_directories(libs/include)

qutim_add_plugin( win7int2
	DISPLAY_NAME "Windows 7 Integration"
	DESCRIPTION "/* TODO */"
	LINK_LIBRARIES w7apiqtwrapper
)
