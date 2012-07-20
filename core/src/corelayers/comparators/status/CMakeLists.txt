qutim_add_plugin(statuscomparator
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/statuscomparator.h
	EXTENSION_CLASS Core::StatusComparator
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Status comparator"
	DESCRIPTION "Default qutIM contacts comparator, which sort by status"
)
