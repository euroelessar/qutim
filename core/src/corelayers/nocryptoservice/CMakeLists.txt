qutim_add_plugin( nocryptoservice
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/nocryptoservice.h
	EXTENSION_CLASS Core::NoCryptoService
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "No crypto"
	DESCRIPTION "Store passwords in plaintext. Easy to read. Easy to recover."
)
