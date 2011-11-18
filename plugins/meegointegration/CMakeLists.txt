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

PKG_CHECK_MODULES(MEEGOTOUCH meegotouchcore)
if(NOT MEEGOTOUCH_FOUND)
	message(STATUS "MEEGOTOUCH libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()

FIND_PACKAGE( Qt4 COMPONENTS QtDBus)
if(NOT QT_QTDBUS_FOUND)
	message(STATUS "QtDBus libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()

pkg_check_modules(QDECLARATIVE_BOOSTABLE qdeclarative-boostable)
if(NOT QDECLARATIVE_BOOSTABLE_FOUND )
	message(STATUS "QDECLARATIVE_BOOSTABLE libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()


FILE( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

qutim_add_extensions_dir(.)
