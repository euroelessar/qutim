qutim_add_artwork(olddelegate)

qutim_add_plugin(oldcontactdelegate
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Old contact delegate"
	DESCRIPTION "Port of contact list delegate from 0.2"
	DEPENDS qutim-simplecontactlist
	LINK_LIBRARIES qutim-simplecontactlist
	QT_LIBRARIES XML
)
