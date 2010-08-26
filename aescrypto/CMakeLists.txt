FIND_PACKAGE (QCA2 REQUIRED)

include_directories(${QCA2_INCLUDE_DIR})

qutim_add_plugin( aescrypto
	LINK_LIBRARIES ${QCA2_LIBRARIES} ${QT_NETWORK_LIBRARIES}
	DISPLAY_NAME "AES crypto service loader"
	DESCRIPTION "Default qutIM crypto implementation. Based on algorithm aes256,"
)
