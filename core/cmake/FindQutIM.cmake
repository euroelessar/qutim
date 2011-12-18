# - Try to find qutIM development headers
# Once done this will define
#
#  QutIM_FOUND - system has qutIM development files
#  QUTIM_INCLUDE_DIR - the qutIM include directory
#  QUTIM_LIBRARY - the qutIM development library
#
# Copyright (c) 2009, Konstantin Plotnikov <kostyapl@gmail.com>
# Copyright (c) 2009-2010, Ruslan Nigmatullin, <euroelessar@gmail.com>

if( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARY )
	# read from cache
	set(QutIM_FOUND TRUE)
else()
	get_filename_component( _qutim_current_dir  "${CMAKE_CURRENT_LIST_FILE}" PATH)
	find_path( QUTIM_INCLUDE_DIR NAMES "qutim/libqutim_global.h" )
	find_package( Qt4 REQUIRED )
	
	if(WIN32)
		find_library( QUTIM_LIBRARY libqutim HINTS ${QUTIM_LIBRARY_DIR} )
	else()
		find_library( QUTIM_LIBRARY qutim HINTS ${QUTIM_LIBRARY_DIR} )
	endif()
		
	if( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARY )
		set( QutIM_FOUND TRUE )
	endif( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARY )
	
	if( QutIM_FOUND )
		if( NOT QutIM_FIND_QUIETLY )
			message( STATUS "Found QutIM: ${QUTIM_LIBRARY}")
		endif()
	else()
		if( QutIM_FIND_REQUIRED )
			if( NOT QUTIM_INCLUDE_DIR )
				message( FATAL_ERROR "Could not find QutIM includes." )
			endif()
			if( NOT QUTIM_LIBRARY )
				message( FATAL_ERROR "Could not find QutIM library." )
			endif()
		else()
			if( NOT QUTIM_INCLUDE_DIR )
				message( STATUS "Could not find QutIM includes." )
			endif()
			if( NOT QUTIM_LIBRARY )
				message( STATUS "Could not find QutIM library." )
			endif()
		endif()
	endif()
	include( "${_qutim_current_dir}/QutIMMacros.cmake" )
endif()
