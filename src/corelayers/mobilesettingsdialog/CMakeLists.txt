qutim_add_plugin( mobilesettingsdialog
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/mobilesettingslayerimpl.h
	EXTENSION_CLASS Core::MobileSettingsLayerImpl
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mobile Settings dialog"
	DESCRIPTION "Kickoff style settings dialog specialy for Maemo/MeeGo"
)
