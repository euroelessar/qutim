cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
if( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
endif(COMMAND cmake_policy )
project( meta-qutim )
set(QT_MIN_VERSION "4.6.0")

INCLUDE(CPackOptions.cmake.in)
INCLUDE(CPack)

set( QUTIM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core" CACHE FILEPATH "Path to qutim sources")
set( QUTIM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/core/libqutim/include" CACHE FILEPATH "Path to qutim headers")
set( QUTIM_LIBRARY libqutim CACHE FILEPATH "Path to qutim library")

set( QUTIM_ADDITIONAL_PATHS "protocols" "integrations" "plugins" )
set( QUTIM_ADDITIONAL_PATHS "${QUTIM_ADDITIONAL_PATHS}" CACHE INTERNAL "" FORCE )

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/core/" "${CMAKE_CURRENT_BINARY_DIR}/core" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/translations/" "${CMAKE_CURRENT_BINARY_DIR}/translations" )

# ADD_CUSTOM_TARGET(uninstall
#   "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

#small simple hack for install Qt dlls and plugins
IF(WIN32)
	SET(QUTIM_REQUIED_DLL_DIR "" CACHE PATH "Path to Qt dll's and plugins. Note: you must add them yourself")
	FILE(GLOB dlls ${QUTIM_REQUIED_DLL_DIR}/*)
	FOREACH(QUTIM_DLL ${dlls})
		IF(IS_DIRECTORY ${QUTIM_DLL})
			INSTALL(DIRECTORY ${QUTIM_DLL}
				DESTINATION bin
			)
		ELSEIF(${QUTIM_DLL} MATCHES ".\\.dll")
			INSTALL(FILES ${QUTIM_DLL}
				DESTINATION bin
			)
		ENDIF()
	ENDFOREACH()
ENDIF(WIN32)
