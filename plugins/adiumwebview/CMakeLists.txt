add_subdirectory(lib)

if( NOT DECLARATIVE_UI )
	qutim_add_plugin( adiumwebview
		GROUP "ChatFactory"
		EXTENSION
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/adiumwebviewfactory.h
		EXTENSION_CLASS "Adium::WebViewFactory, Core::AdiumChat::ChatViewFactory"
		DISPLAY_NAME "Adium-like WebView"
		DESCRIPTION "Well-look stylished view based on WebKit and Adium ideas"
		DEPENDS adiumchat
		LINK_LIBRARIES qutim-adiumwebview
		QT_LIBRARIES WEBKIT
	)
endif()

qutim_add_artwork( data )
