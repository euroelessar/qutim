#qutim_check_component(Bearer)

if(NOT QT_MOBILITY_BEARER_FOUND)
	message(STATUS "Mobility bearer manager isn't found. Using bearer from QtNetwork")
	set(QT_MOBILITY_BEARER_LIBRARY ${QT_QTNETWORK_LIBRARY})
	set(QT_MOBILITY_BEARER_INCLUDE_DIR ${QT_QTNETWORK_INCLUDE_DIR})
endif()

qutim_add_plugin(bearermanager
	DISPLAY_NAME "Network bearer Manager"
	DESCRIPTION "Used to monitor the availability of network."
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/bearermanager.h
	EXTENSION_CLASS BearerManager
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	LINK_LIBRARIES ${QT_MOBILITY_BEARER_LIBRARY}
	INCLUDE_DIRS ${QT_MOBILITY_BEARER_INCLUDE_DIR}
)
