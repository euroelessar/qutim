qutim_add_plugin( localization
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/localizationmodule.h
	EXTENSION_CLASS Core::LocalizationModule,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Localization"
	DESCRIPTION "Default qutIM localization engine"
)
