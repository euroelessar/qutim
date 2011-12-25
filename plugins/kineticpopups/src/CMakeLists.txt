qutim_add_plugin(kineticpopups
	EXTENSION
	EXTENSION_CLASS KineticPopups::Backend
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/backend.h
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Kinetic popups"
	DESCRIPTION "Growl-like popups, based on power of Qt Quick"
	COMPILE_FLAGS "-DKINETICPOPUPS_LIBRARY"
	QT_LIBRARIES DECLARATIVE
)

QUTIM_ADD_ARTWORK(quickpopup)
