if(MAEMO5) #simple hack for broken maemo 5 QtDeclarative definition
SET(QT_QTDECLARATIVE_LIBRARY "/usr/lib/libQtDeclarative.so")
SET(QT_QTDECLARATIVE_INCLUDE_DIR "/usr/include/QtDeclarative/")
SET(QT_QTDECLARATIVE_FOUND true)
endif()

QUTIM_ADD_ARTWORK(qmlchat)

qutim_add_plugin(qmlchat
	GROUP "ChatFactory"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/quickchatviewfactory.h
	EXTENSION_CLASS "Core::AdiumChat::QuickChatViewFactory, Core::AdiumChat::ChatViewFactory"
	DISPLAY_NAME "Qt Quick view (experimental)"
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DESCRIPTION "Modern chat view based on power of QtQuick"
	DEPENDS adiumchat
	QT_LIBRARIES DECLARATIVE OPENGL
)


