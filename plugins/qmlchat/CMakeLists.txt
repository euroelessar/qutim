FIND_PACKAGE( Qt4 COMPONENTS QtDeclarative QtOpenGL)

QUTIM_ADD_ARTWORK(qmlchat)

qutim_add_plugin(qmlchat
	GROUP "ChatFactory"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/quickchatviewfactory.h
	EXTENSION_CLASS Core::AdiumChat::QuickChatViewFactory
	DISPLAY_NAME "Qt Quick view (experimental)"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Modern chat view based on power of QtQuick"
	DEPENDS adiumchat
	LINK_LIBRARIES adiumchat ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTOPENGL_LIBRARY}
	INCLUDE_DIRS ${QT_QTDECLARATIVE_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR}
)


