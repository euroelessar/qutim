# qutIM KDE Integration

find_package(KDE4)
if(NOT KDE4_FOUND)
	message(STATUS "Cannot find KDE4, kde integration plugin will be skipped")
	return()
endif()

include(KDE4Defaults)

qutim_add_plugin(kdeintegration
	${KDE4_PLUGIN_SRC}
	DISPLAY_NAME "KDE integration"
	DESCRIPTION "Integration with K Desktop Environment"
	LINK_LIBRARIES ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS}
	INCLUDE_DIRS ${KDE4_INCLUDES}
	)

add_definitions( ${KDE4_DEFINITIONS} )
