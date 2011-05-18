qutim_add_plugin( adiumsrvicons
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/serviceicons.h
	EXTENSION_CLASS Adium::ServiceIcons,qutim_sdk_0_3::IconWrapper
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Adium service icons"
	DESCRIPTION "Protocol icons powered by Adium ServiceIcons packs"
)
