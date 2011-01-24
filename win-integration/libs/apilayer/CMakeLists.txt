cmake_minimum_required(VERSION 2.6)
project("WinThings API layer library")

if(MSVC)
	file(GLOB_RECURSE wt_apil_headers "src/*.h")
	file(GLOB_RECURSE wt_apil_sources "src/*.cpp")
	add_library(wapicwrapper SHARED ${wt_apil_headers} ${wt_apil_sources})
	set_target_properties(wapicwrapper PROPERTIES COMPILE_FLAGS "/DWT_APIL_LIB /Zc:wchar_t-") # do not treat wchar_t as built-in type
	target_link_libraries(wapicwrapper "dwmapi.lib")
	set(WINTHINGS_APILAYER_LIBRARY wapicwrapper CACHE INTERNAL "")

	install( TARGETS wapicwrapper
		RUNTIME DESTINATION bin
			COMPONENT wapicwrapper
		LIBRARY DESTINATION lib${LIB_SUFFIX}
			COMPONENT wapicwrapper
		ARCHIVE DESTINATION lib${LIB_SUFFIX}
			COMPONENT wapicwrapper
	)


else()
	if(NOT WINTHINGS_APILAYER_LIBRARY)
		if(MINGW)
			message(STATUS "WinThings API layer library cannot be built by MinGW.")
		else()
			message(STATUS "WinThings API layer library cannot be built by your compiler.")
		endif()
		set(WINTHINGS_APILAYER_LIBRARY "WINTHINGS_APILAYER_LIBRARY-NOTFOUND" CACHE FILEPATH "Path to prebuilt api-layer library of WinThings.")
		message(STATUS "You should specify WINTHINGS_APILAYER_LIBRARY if you have prebuild library.")
	endif()
endif()
