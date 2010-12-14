cmake_minimum_required(VERSION 2.8)
project("Win7API Qt Wrapper")

FIND_PACKAGE (Qt4 REQUIRED)
INCLUDE(UseQt4)

file(GLOB_RECURSE apis_sources    "apilayer/src/*.cpp")
file(GLOB_RECURSE apis_headers    "apilayer/src/*.h")
file(GLOB_RECURSE classes_sources "qtwrapper/src/*.cpp")
file(GLOB_RECURSE classes_headers "qtwrapper/src/*.h")

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-") # handling wchar_t as not as built-in type
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} dwmapi.lib")
	add_library(w7apicwrapper SHARED ${apis_sources} ${apis_headers})
	set_target_properties(w7apicwrapper PROPERTIES COMPILE_FLAGS "/DW7CLDLL")

	QT4_WRAP_CPP (MOC_SRCS ${classes_headers})
	add_library(w7apiqtwrapper SHARED ${classes_sources} ${classes_headers} ${MOC_SRCS})
	target_link_libraries(w7apiqtwrapper w7apicwrapper ${QT_LIBRARIES})
	set_target_properties(w7apiqtwrapper PROPERTIES COMPILE_FLAGS "/DW7QTWRAPPER")
endif(MSVC)

if(MINGW)
	SET(APILAYER_DLL_PATH "" CACHE FILEPATH "Path to libw7apiqtwrapper.")
	QT4_WRAP_CPP (MOC_SRCS ${classes_headers})
	add_library(w7apiqtwrapper SHARED ${classes_sources} ${classes_headers} ${MOC_SRCS})
	target_link_libraries(w7apiqtwrapper ${QT_LIBRARIES} ${APILAYER_DLL_PATH})
	set_target_properties(w7apiqtwrapper PROPERTIES COMPILE_FLAGS "-DW7QTWRAPPER")
endif(MINGW)
