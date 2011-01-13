cmake_minimum_required(VERSION 2.8)
project("WinThings")

FIND_PACKAGE (Qt4 REQUIRED)
INCLUDE(UseQt4)

if(MSVC)
	file(GLOB_RECURSE apis_sources    "apilayer/src/*.cpp")
	file(GLOB_RECURSE apis_headers    "apilayer/src/*.h")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-") # handling wchar_t as not as built-in type
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} dwmapi.lib")
	add_library(wapicwrapper SHARED ${apis_sources} ${apis_headers})
	set_target_properties(wapicwrapper PROPERTIES COMPILE_FLAGS "/DW7CLDLL")
	set(WINTHINGS_LIBRARIES wapicwrapper)
endif()

file(GLOB_RECURSE classes_sources "qtwrapper/src/*.cpp")
file(GLOB_RECURSE classes_headers "qtwrapper/src/*.h")
QT4_WRAP_CPP (wapiqt_MOC_SRCS ${classes_headers})
add_library(wapiqtwrapper SHARED ${classes_sources} ${classes_headers} ${wapiqt_MOC_SRCS})
target_link_libraries(wapiqtwrapper ${WINTHINGS_LIBRARIES} ${QT_LIBRARIES})

if(MSVC)
	set_target_properties(wapiqtwrapper PROPERTIES COMPILE_FLAGS "/DW7QTWRAPPER")
else()
	set_target_properties(wapiqtwrapper PROPERTIES COMPILE_FLAGS "-DW7QTWRAPPER")
endif()
