# - Try to find qutIM development headers
# Once done this will define
#
#  QutIM_FOUND - system has qutIM development files
#  QUTIM_INCLUDE_DIR - the qutIM include directory
#  QUTIM_LIBRARIES - the qutIM development library
#
# Copyright (c) 2009, Konstantin Plotnikov <kostyapl@gmail.com>
# Copyright (c) 2009-2010, Ruslan Nigmatullin, <euroelessar@gmail.com>

if( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARIES )

    # read from cache
    set(QutIM_FOUND TRUE)

else( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARIES )
  
    find_path( QUTIM_INCLUDE_DIR NAMES "qutim/libqutim_global.h" )
    find_library( QUTIM_LIBRARIES qutim PATHS ${QUTIM_LIBRARIES_DIR} )
    
    if( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARIES )
	set( QutIM_FOUND TRUE )
    endif( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARIES )
    
    if(QutIM_FOUND)
	if(NOT QutIM_FIND_QUIETLY)
	    message(STATUS "Found QutIM: ${QUTIM_LIBRARIES}")
	endif(NOT QutIM_FIND_QUIETLY)
    else(QutIM_FOUND)
	if(QutIM_FIND_REQUIRED)
	    if(NOT QUTIM_INCLUDE_DIR)
		message(FATAL_ERROR "Could not find QutIM includes.")
	    endif(NOT QUTIM_INCLUDE_DIR)
	    if(NOT QUTIM_LIBRARIES)
		message(FATAL_ERROR "Could not find QutIM library.")
	    endif(NOT QUTIM_LIBRARIES)
	else(QutIM_FIND_REQUIRED)
	    if(NOT QUTIM_INCLUDE_DIR)
		message(STATUS "Could not find QutIM includes.")
	    endif(NOT QUTIM_INCLUDE_DIR)
	    if(NOT QUTIM_LIBRARIES)
		message(STATUS "Could not find QutIM library.")
	    endif(NOT QUTIM_LIBRARIES)
	endif(QutIM_FIND_REQUIRED)
    endif(QutIM_FOUND)
	
endif( QUTIM_INCLUDE_DIR AND QUTIM_LIBRARIES )

INCLUDE(QutIMMacros)

