qutim_add_artwork(olddelegate)

qutim_add_plugin(oldcontactdelegate
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Old contact delegate"
	DESCRIPTION "Port of contact list delegate from 0.2"
	DEPENDS simplecontactlist
	LINK_LIBRARIES simplecontactlist
	QT_LIBRARIES XML
)
