IF (UNIX AND NOT APPLE AND NOT SYMBIAN)
	include( FindPkgConfig )
	pkg_check_modules (XSS xscrnsaver)
	IF( XSS_FOUND )
		ADD_DEFINITIONS( -DHAVE_XSS )
		FIND_LIBRARY( XSS_LIB NAMES Xss PATHS ${XSS_LIBDIR} )
	ELSE( XSS_FOUND )
		message(STATUS "Warning: libxss not found, idle detection won't be accurate")
	ENDIF( XSS_FOUND )
ENDIF (UNIX AND NOT APPLE AND NOT SYMBIAN)

qutim_add_plugin( idledetector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/idle.h
	EXTENSION_CLASS Psi::Idle
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Idle detector"
	DESCRIPTION "Native idle detector, based on system api"
	LINK_LIBRARIES ${XSS_LIB}
	INCLUDE_DIRS ${XSS_INCLUDEDIR}
)
