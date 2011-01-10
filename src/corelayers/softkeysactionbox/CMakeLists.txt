qutim_add_plugin( softkeysactionbox
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/softkeysactionbox.h
	EXTENSION_CLASS Core::SoftkeysActionBoxGenerator,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "SoftkeysActionBox"
	DESCRIPTION "Actionbox module for mobile devices with softkeys support"
)
