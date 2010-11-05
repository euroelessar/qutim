find_package( Qt4 COMPONENTS QtWebkit )

qutim_add_plugin( adiumchat
	SUBPLUGIN
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Webkit chat layer"
	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
	LINK_LIBRARIES ${QT_QTWEBKIT}
	COMPILE_FLAGS "-DADIUMCHAT_LIBRARY"
)

#qutim_add_plugin( adiumchatlayer
#	EXTENSION
#	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/chatlayer/chatlayerimpl.h
#	EXTENSION_CLASS Core::AdiumChat::ChatLayerImpl
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
#	DISPLAY_NAME "Webkit chat layer"
#	DESCRIPTION "Default qutIM chat implementation, based on Adium chat styles"
#)

#qutim_add_plugin( adiumchatform
#	EXTENSION
#	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/adiumchatform/adiumchatform.h
#	EXTENSION_CLASS Core::AdiumChat::AdiumChatForm
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/adiumchatform
#	LINK_LIBRARIES adiumchatlayer
#	DEPENDS adiumchatlayer
#	DISPLAY_NAME "Adium"
#	DESCRIPTION "Adium-like chatform implementation for adiumchat"
#)

#qutim_add_plugin( classicchatform
#	EXTENSION
#	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/classicchatform/classicchatform.h
#	EXTENSION_CLASS Core::AdiumChat::ClassicChatForm
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/classicchatform
#	LINK_LIBRARIES adiumchatlayer
#	DEPENDS adiumchatlayer
#	DISPLAY_NAME "Classic"
#	DESCRIPTION "Default qutIM chatform implementation for adiumchat"
#)

#qutim_add_plugin( adiumchatsettings
#	EXTENSION
#	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/settings/chatsettings.h
#	EXTENSION_CLASS Core::ChatSettings,qutim_sdk_0_3::StartupModule
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/settings
#	LINK_LIBRARIES adiumchatlayer
#	DEPENDS adiumchatlayer
#	DISPLAY_NAME "Chat settings"
#	DESCRIPTION ""
#)

#qutim_add_plugin( adiumtimemodifier
#	EXTENSION
#	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/timemodifier/timemodifier.h
#	EXTENSION_CLASS Core::AdiumChat::TimeModifier,Core::AdiumChat::MessageModifier
#	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/timemodifier
#	LINK_LIBRARIES adiumchatlayer
#	DEPENDS adiumchatlayer
#	DISPLAY_NAME "Time modifier"
#	DESCRIPTION "Modifier for %time% and %timeOpened% param, based on Apple's documentation"
#)
