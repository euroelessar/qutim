# qutIM KDE Integration

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( KDEIntegration )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( Qt4 REQUIRED )
find_package( KDE4 REQUIRED )
include( KDE4Defaults )

macro_ensure_version( "4.2.0" "${GENERIC_LIB_VERSION}" KDE_HAS_4_2 )
if( NOT KDE_HAS_4_2 )
	message( SEND_ERROR "Minimum kdelibs version is 4.2.0, but ${GENERIC_LIB_VERSION} found" )
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
