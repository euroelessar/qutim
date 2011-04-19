# qutIM KDE Integration

find_package(KDE4)
if(NOT KDE4_FOUND)
	message(STATUS "Cannot find KDE4, kde integration plugin will be skipped")
	return()
endif()

include( KDE4Defaults )

macro_ensure_version( "4.2.0" "${GENERIC_LIB_VERSION}" KDE_HAS_4_2 )
if( NOT KDE_HAS_4_2 )
	message(STATUS "Minimum kdelibs version is 4.2.0, but ${GENERIC_LIB_VERSION} found" )
	return()
endif( NOT KDE_HAS_4_2 )


#qt4_add_dbus_adaptor( KDE4_PLUGIN_SRC
#	"${CMAKE_CURRENT_SOURCE_DIR}/src/dbusadaptors/org.kde.StatusNotifierItem.xml"
#	"${CMAKE_CURRENT_BINARY_DIR}/src/tray/kdetrayicondbus.h"
#	KdeTrayIconDBus
#	)

qutim_add_plugin(kdeintegration
	${KDE4_PLUGIN_SRC}
	DISPLAY_NAME "KDE integration"
	DESCRIPTION "Integration with K Desktop Environment"
	LINK_LIBRARIES ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS}
	INCLUDE_DIRS ${KDE4_INCLUDES}
	)

add_definitions( ${KDE4_DEFINITIONS} )
