# qutIM KDE Integration

if(${KDEINTEGRATION} EQUAL 0 OR ${KDEINTEGRATION} STREQUAL "OFF")

else()
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

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

	set( QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources" )
	list( APPEND CMAKE_MODULE_PATH ${QUTIM_PATH}/cmake )
	find_package( QutIM REQUIRED )

	#qt4_add_dbus_adaptor( KDE4_PLUGIN_SRC
	#	"${CMAKE_CURRENT_SOURCE_DIR}/src/dbusadaptors/org.kde.StatusNotifierItem.xml"
	#	"${CMAKE_CURRENT_BINARY_DIR}/src/tray/kdetrayicondbus.h"
	#	KdeTrayIconDBus
	#	)
	include_directories( ${QUTIM_INCLUDE_DIR} )
endif()

qutim_add_plugin( kdeintegration
	${KDE4_PLUGIN_SRC}
	DISPLAY_NAME "KDE integration"
	DESCRIPTION "Integration with K Desktop Environment"
	LINK_LIBRARIES ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS}
	)

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )

include_directories( ${QT_QTGUI_INCLUDE_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${KDE4_INCLUDES}
	 )
