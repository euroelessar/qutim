cmake_minimum_required(VERSION 2.8)
project("WinThings Qt-wrapper library")

FIND_PACKAGE(Qt4)
INCLUDE(${QT_USE_FILE})

file(GLOB_RECURSE wt_qtw_headers "src/*.h")
file(GLOB_RECURSE wt_qtw_sources "src/*.cpp")
QT4_WRAP_CPP(wt_qtw_mocs ${wt_qtw_headers})

if(WINTHINGS_APILAYER_LIBRARY)
	add_library(wapiqtwrapper SHARED ${wt_qtw_headers} ${wt_qtw_sources} ${wt_qtw_mocs})
	target_link_libraries(wapiqtwrapper ${WINTHINGS_APILAYER_LIBRARY} ${QT_LIBRARIES})
	set_target_properties(wapiqtwrapper PROPERTIES DEFINE_SYMBOL "WT_QT_WRAPPER")
	if(MSVC)
		set_target_properties(wapiqtwrapper PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-")
	endif()
endif()
