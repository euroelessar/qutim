qutim_add_plugin( proxysettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/proxysettings.h
	EXTENSION_CLASS Core::ProxySettings,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Proxy settings"
	DESCRIPTION "Default implementation of proxy settings widget"
)
