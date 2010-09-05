macro_ensure_version("4.7.0" "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" QT_HAS_4_7 )
IF(NOT QT_HAS_4_7)
	message("Can't build Vkontakte photoalbum" )
	return()
ENDIF()

find_package( Qt4 COMPONENTS QtDeclarative QtOpenGL )

qutim_add_plugin( vphotoalbum
	SUBPLUGIN
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Vkontakte photoalbum support"
	DESCRIPTION "Vkontakte photoalbum support for qutIM, based on power of Qt Quick"
	LINK_LIBRARIES vkontakte ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTOPENGL_LIBRARY}
)

INSTALL( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vphotoalbum" DESTINATION share/qutim)
