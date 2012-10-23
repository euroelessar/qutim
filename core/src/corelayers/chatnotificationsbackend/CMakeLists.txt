qutim_add_plugin( chatnotificationsbackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/chatnotificationsbackend.h
	EXTENSION_CLASS Core::ChatNotificationsBackend,Ureen::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Chat notifications backend"
	DESCRIPTION "Prints information about events into chats"
)
