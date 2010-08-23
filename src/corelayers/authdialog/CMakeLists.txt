qutim_add_plugin( authdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/authdialogimpl.h
	EXTENSION_CLASS Core::AuthDialogImpl
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Authorization dialog"
	DESCRIPTION "Simple authorization dialog"
)
