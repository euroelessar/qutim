# - Try to find qutIM development headers
# Once done this will define
#
#  QUTIM_FOUND - system has qutIM headers
#  QUTIM_INCLUDE_DIR - the qutIM include directory
#
# Copyright (c) 2009, Ruslan Nigmatullin, <euroelessar@gmail.com>

if(QUTIM_INCLUDE_DIR)
    # Already in cache, be silent
    set(QUTIM_FIND_QUIETLY TRUE)	
endif(QUTIM_INCLUDE_DIR)

if(NOT QUTIM_DO_NOT_FIND )
    find_path( QUTIM_INCLUDE_DIRS qutim/protocolinterface.h ../../include )

    if( QUTIM_INCLUDE_DIRS )
	if(NOT QUTIM_FIND_QUIETLY )
	    message( STATUS "Found qutIM headers: ${QUTIM_INCLUDE_DIRS}/qutim" )
	endif(NOT QUTIM_FIND_QUIETLY )
    else( QUTIM_INCLUDE_DIRS )
	if( QUTIM_FIND_REQUIRED )
	    message( FATAL_ERROR "Could not find qutIM development headers" )
	else( QUTIM_FIND_REQUIRED )
	    message( STATUS "Could not find qutIM development headers" )
	endif( QUTIM_FIND_REQUIRED )
    endif( QUTIM_INCLUDE_DIRS )
endif(NOT QUTIM_DO_NOT_FIND )

# Thanks to KDE project
macro( qutim_add_ui_files _sources )
   foreach( _current_FILE ${ARGN} )

      get_filename_component( _tmp_FILE ${_current_FILE} ABSOLUTE )
      get_filename_component( _basename ${_tmp_FILE} NAME_WE )
      set( _header ${CMAKE_CURRENT_BINARY_DIR}/ui_${_basename}.h )

      # we need to run uic and replace some things in the generated file
      # this is done by executing the cmake script qutruic.cmake
      add_custom_command( OUTPUT ${_header}
         COMMAND ${CMAKE_COMMAND}
         ARGS
         -DQUTIM_UIC_EXECUTABLE:FILEPATH=${QT_UIC_EXECUTABLE}
         -DQUTIM_UIC_FILE:FILEPATH=${_tmp_FILE}
         -DQUTIM_UIC_H_FILE:FILEPATH=${_header}
         -DQUTIM_UIC_BASENAME:STRING=${_basename}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/qutimuic.cmake
         MAIN_DEPENDENCY ${_tmp_FILE}
      )
      list( APPEND ${_sources} ${_header} )
   endforeach (_current_FILE)
endmacro( qutim_add_ui_files )
