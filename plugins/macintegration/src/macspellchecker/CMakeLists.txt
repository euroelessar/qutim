find_library(COCOA_LIBRARIES Cocoa)

qutim_add_plugin(macspellchecker
	EXTENSION
	EXTENSION_CLASS MacIntegration::MacSpellChecker
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/macspellchecker.h
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "SpellChecker"
	DESCRIPTION "Mac OS X spell checker"
	LINK_LIBRARIES ${COCOA_LIBRARIES}
) 
