IF (UNIX AND NOT APPLE AND NOT SYMBIAN AND NOT MAEMO5)
	include( FindPkgConfig )
	pkg_check_modules (XSS xscrnsaver)
	IF( XSS_FOUND )
		ADD_DEFINITIONS( -DHAVE_XSS )
		FIND_LIBRARY( XSS_LIB NAMES Xss PATHS ${XSS_LIBDIR} )
	ELSE( XSS_FOUND )
		message(STATUS "Warning: libxss not found, idle detection won't be accurate")
	ENDIF( XSS_FOUND )
ENDIF (UNIX AND NOT APPLE AND NOT SYMBIAN AND NOT MAEMO5)

IF (MAEMO5)
FIND_PACKAGE( Qt4 COMPONENTS QtDBus)
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "Cannot build Maemo5IdleDetector")
	return()
endif()
ENDIF (MAEMO5)

qutim_add_plugin( idledetector
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/idle.h
	EXTENSION_CLASS Psi::Idle
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Idle detector"
	DESCRIPTION "Native idle detector, based on system api"
	LINK_LIBRARIES ${XSS_LIB} ${QT_QTDBUS_LIBRARY}
	INCLUDE_DIRS ${XSS_INCLUDEDIR} ${QT_QTDBUS_INCLUDE_DIR}
)
