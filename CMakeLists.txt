#Plugin manager
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (plugman)

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

SET (QT_USE_QTNETWORK true)
SET (QT_USE_QTGUI true)
SET (QT_USE_QTXML true)
FIND_PACKAGE (Qt4 REQUIRED)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


INCLUDE (${CMAKE_ROOT}/Modules/UseQt4.cmake)
FIND_PATH( QUTIM_INCLUDE_DIRS qutim/protocolinterface.h ../../include )
if( QUTIM_INCLUDE_DIRS )
	MESSAGE( STATUS "Found qutIM headers: ${QUTIM_INCLUDE_DIRS}" )
	INCLUDE_DIRECTORIES( ${QUTIM_INCLUDE_DIRS} )
else( QUTIM_INCLUDE_DIRS )
	MESSAGE( SEND_ERROR "Could NOT find qutIM development headers" )
endif( QUTIM_INCLUDE_DIRS )
if( APPLE )
	INCLUDE_DIRECTORIES ( /opt/local/include )
endif( APPLE )

include( FindZLIB )

if( ZLIB_FOUND )
        LIST(APPEND PLUGMAN_LIBS ${ZLIB_LIBRARIES} )
        LIST(APPEND PLUGMAN_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} )
endif( ZLIB_FOUND)

INCLUDE_DIRECTORIES (${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
	${PLUGMAN_INCLUDE_DIR}
        .
		src
		libs
        )

add_subdirectory (libs)

SET (QT_HEADERS
	src/plugpackagehandler.h
	src/plugmanager.h
	src/plugitemdelegate.h
	src/collisionprotect.h
	src/plugman.h
	src/plugmansettings.h
	src/plugdownloader.h
	src/pluginstaller.h
	src/plugxmlhandler.h
	src/plugpackagemodel.h
	src/plugpackageitem.h
)
SET(HEADERS
${QT_HEADERS}
	src/mirror.h
	src/plugpackage.h	
)

SET (SOURCES 
	src/mirror.cpp
	src/plugpackagehandler.cpp
	src/plugmanager.cpp
	src/plugitemdelegate.cpp
	src/collisionprotect.cpp 
	src/plugxmlhandler.cpp
	src/plugman.cpp
	src/plugmansettings.cpp
	src/plugdownloader.cpp
	src/pluginstaller.cpp
	src/plugpackagemodel.cpp
	src/plugpackage.cpp
	src/plugpackageitem.cpp
)

SET (RESOURCES
	icons.qrc
)

SET (FORMS
	ui/settings.ui
	ui/manager.ui
)

QT4_WRAP_CPP (MOC_SRCS ${QT_HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

ADD_LIBRARY (plugman SHARED ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})

TARGET_LINK_LIBRARIES (plugman
        ${QT_LIBRARIES}
        ${PLUGMAN_LIBS}
		osdabzip
        )


INSTALL (TARGETS plugman DESTINATION lib/qutim )
