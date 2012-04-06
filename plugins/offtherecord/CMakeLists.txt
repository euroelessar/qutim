find_package( PkgConfig )
pkg_check_modules( OTR libotr )

qutim_add_plugin( offtherecord
	DISPLAY_NAME "Off-the-Record Messaging"
	DESCRIPTION "Plugin provides encryption of messages by OTR"
	LINK_LIBRARIES ${OTR_LIBRARIES}
	INCLUDE_DIRS ${OTR_INCLUDE_DIRS}
)
