if(NOT MAEMO6)
	message(STATUS "Meego Harmattan integration will be skipped")
	return()
endif()

find_package( Qt4 COMPONENTS QtDeclarative QtOpenGL)
if(NOT QT_QTDECLARATIVE_FOUND OR NOT QT_QTOPENGL_FOUND)
	message(STATUS "Declarative libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(QMSYSTEM qmsystem2)
if(NOT QMSYSTEM_FOUND)
	message(STATUS "QMSYSTEM libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()


FILE( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

qutim_add_extensions_dir(.)
