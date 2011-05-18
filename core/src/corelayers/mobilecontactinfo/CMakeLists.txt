qutim_add_plugin( mobilecontactinfo
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/mobilecontactinfo.h
	EXTENSION_CLASS Core::MobileContactInfo
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mobile information window about a contact"
	DESCRIPTION "qutIM implementation of the information window for mobiles"
)
