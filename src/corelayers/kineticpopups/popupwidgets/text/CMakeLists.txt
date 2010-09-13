qutim_add_plugin(textpopupwidget
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/textpopup.h
	EXTENSION_CLASS Core::KineticPopups::TextPopupWidget
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Text Popups"
	DESCRIPTION "Standart textbrowser based popups"
	DEPENDS kineticpopups
	LINK_LIBRARIES kineticpopups
)

QUTIM_ADD_ARTWORK(textpopups)
