list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

find_package(QtMobility)
if(NOT QT_MOBILITY_FOUND)
	message(STATUS "Mobility plugin set will be skipped")
	return()
endif()

macro(qutim_check_component component)
	STRING(TOUPPER ${component} _COMPONENT)
	IF(NOT ${QT_MOBILITY_${_COMPONENT}_FOUND})
		message("Module ${component} not found. ${component} support disabled.")
		return()
	ENDIF()
endmacro()

include_directories(${QT_MOBILITY_INCLUDE_DIR} ${QT_MOBILITY_PARENT_INCLUDE_DIR})

qutim_add_extensions_dir(src)
