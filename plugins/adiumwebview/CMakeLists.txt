add_subdirectory(lib)

qutim_add_plugin( adiumwebview
	GROUP "ChatFactory"
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/adiumwebviewfactory.h
	EXTENSION_CLASS Adium::WebViewFactory
	DISPLAY_NAME "Adium-like WebView"
	DESCRIPTION "Well-look stylished view based on WebKit and Adium ideas"
	DEPENDS adiumchat
	LINK_LIBRARIES qutim-adiumwebview adiumchat
	QT_LIBRARIES WEBKIT
)

qutim_add_artwork( data )
