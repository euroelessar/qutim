find_package( ASPELL REQUIRED )
include_directories( ${ASPELL_INCLUDE_DIR} )

qutim_add_plugin( aspeller
	DISPLAY_NAME "ASpell checker"
	DESCRIPTION "Check spelling by ASpell library"
	LINK_LIBRARIES ${ASPELL_LIBRARIES}
)
