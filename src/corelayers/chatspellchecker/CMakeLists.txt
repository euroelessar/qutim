qutim_add_plugin( chatspellchecker
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/chatspellchecker.h
	EXTENSION_CLASS Core::ChatSpellChecker,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Chat spell checker"
	DESCRIPTION "Highlights grammar errors and provides spelling suggestions for misspelled words in chats"
)
