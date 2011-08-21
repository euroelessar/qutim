if(NOT MAEMO6)
	message(STATUS "Meego Harmattan integration will be skipped")
	return()
endif()

find_package( Qt4 COMPONENTS QtDeclarative )
if(NOT QT_QTDECLARATIVE_FOUND)
	message(STATUS "Declarative libraries not found. Meego Harmattan integration will be skipped")
	return()
endif()


FILE( GLOB PLUGINS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

qutim_add_extensions_dir(.)
