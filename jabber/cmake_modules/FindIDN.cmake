# cmake macro to test IDN library

# Copyright (c) 2006, Will Stephenson <wstephenson@kde.org>

#  IDN_FOUND - Test has found IDN dependencies
#  IDN_INCLUDE_DIR - Include needed for IDN
#  IDN_LIBRARIES - Libraries needed for IDN
#  IDN_DEFINITIONS - Compiler switches required for using IDN

# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (IDN_INCLUDEDIR AND IDN_LIBRARIES)
	# cached
	set(IDN_INCLUDE_DIR ${IDN_INCLUDEDIR})
	SET(IDN_FOUND TRUE)
else (IDN_INCLUDEDIR AND IDN_LIBRARIES)
	if (NOT WIN32 AND NOT APPLE)
		find_package(PkgConfig)
		if(PKG_CONFIG_EXECUTABLE)
			pkg_check_modules(IDN libidn)
		endif(PKG_CONFIG_EXECUTABLE)
	endif (NOT WIN32 NOT APPLE)

	if(NOT IDN_FOUND)
		find_path(IDN_INCLUDEDIR idna.h)
		find_library(IDN_LIBRARIES NAMES idn libidn idn-11 libidn-11)
		if (IDN_INCLUDEDIR AND IDN_LIBRARIES)
			SET(IDN_FOUND TRUE)
		endif (IDN_INCLUDEDIR AND IDN_LIBRARIES)
	endif(NOT IDN_FOUND)

	if(IDN_FOUND)
		set(IDN_DEFINITIONS ${IDN_CFLAGS})
		if(NOT IDN_FIND_QUIETLY)
			message(STATUS "Found libidn: ${IDN_LIBRARIES}")
		endif(NOT IDN_FIND_QUIETLY)
		set(IDN_INCLUDE_DIR ${IDN_INCLUDEDIR})
		mark_as_advanced( IDN_INCLUDE_DIR )
	else(IDN_FOUND)
		if (IDN_FIND_REQUIRED)
			message(FATAL_ERROR "Not found required libidn")
		endif (IDN_FIND_REQUIRED)
	endif(IDN_FOUND)
endif (IDN_INCLUDEDIR AND IDN_LIBRARIES)
