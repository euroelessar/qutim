qutim_add_plugin( mobileaboutdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/mobileaboutcreator.h
	EXTENSION_CLASS Core::MobileAboutCreator
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mobile about dialog"
	DESCRIPTION "Mobile dialog about qutIM and it's authors"
)
