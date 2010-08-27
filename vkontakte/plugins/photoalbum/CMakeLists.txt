find_package( Qt4 COMPONENTS QtDeclarative QtOpenGL )

qutim_add_plugin( vphotoalbum
	SUBPLUGIN
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Vkontakte photoalbum support"
	DESCRIPTION "Vkontakte photoalbum support for qutIM, based on power of Qt Quick"
	LINK_LIBRARIES vkontakte ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTOPENGL_LIBRARY}
)
