if(NOT APPLE)
	message(STATUS "Growl plugin will be skipped")
	return()
endif()

find_library(GROWL_LIBRARIES Growl)
find_library(COCOA_LIBRARIES Cocoa)

qutim_add_plugin( macintegration
	DISPLAY_NAME "MacIntegration"
	DESCRIPTION "PLugins collection for integration with Mac OS X"
	INCLUDE_DIRS ${GROWL_LIBRARIES}/Headers
	LINK_LIBRARIES ${GROWL_LIBRARIES} ${COCOA_LIBRARIES}
)

#reinder, please update this paths
#INSTALL(DIRECTORY share/config
#	DESTINATION share/qutim
#		COMPONENT Core
#)
