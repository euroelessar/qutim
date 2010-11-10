qutim_add_plugin( timemodifier
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/timemodifier.h
	EXTENSION_CLASS Core::TimeModifier,MessageModifier
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Time modifier"
	DESCRIPTION "Modifier for %time% and %timeOpened% param, based on Apple's documentation"
)
