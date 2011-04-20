

qutim_add_plugin( kineticpopups
	SUBPLUGIN
	COMPILE_FLAGS "-DKINETICPOPUPS_LIBRARY"
	DISPLAY_NAME "Kinetic popups"
	DESCRIPTION "Modern popups, based on power of Qt Quick"
)

qutim_add_extensions_dir(popupwidgets)
