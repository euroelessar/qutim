#qutim_check_component(VibroKit)

if(NOT QT_MOBILITY_FEEDBACK_FOUND)
	message(STATUS "Cannot build vibration backend")
	return()
endif()


qutim_add_plugin(vibrobackend
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/vibrobackend.h
	EXTENSION_CLASS Core::VibroBackend
	LINK_LIBRARIES ${QT_MOBILITY_FEEDBACK_LIBRARY}
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Vibro engine"
	DESCRIPTION "Vibro engine based on QtFeedback"
	INCLUDE_DIRS ${QT_MOBILITY_FEEDBACK_INCLUDE_DIR}
)
