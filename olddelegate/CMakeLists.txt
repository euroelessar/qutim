find_package( Qt4 COMPONENTS QXml )

qutim_add_artwork(olddelegate)

qutim_add_plugin(oldcontactdelegate
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/contactlistitemdelegate.h
	EXTENSION_CLASS ContactListItemDelegate
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Old contact delegate"
	DESCRIPTION "Port of contact list delegate from 0.2"
)
