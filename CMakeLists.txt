CMAKE_MINIMUM_REQUIRED (VERSION 2.6 FATAL_ERROR)
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT (qutim)
SET(QT_MIN_VERSION "4.4.0")

ADD_DEFINITIONS ( -DBUILD_QUTIM )

if( UNIX )
	if( BSD )
		SET( CMAKE_THREAD_LIBS -pthread )
		SET( CMAKE_USE_PTHREADS ON )
		SET( CMAKE_EXE_LINKER_FLAGS -pthread )
	endif( BSD )
endif( UNIX )

if(NOT REVISION )
    find_path(REVISION_ENTRY "entries" "${CMAKE_CURRENT_SOURCE_DIR}/.svn")
    if( REVISION_ENTRY )
	file(STRINGS "${REVISION_ENTRY}/entries"
	    QUTIM_SVN_INFO
	    LIMIT_COUNT 4)
	list(GET QUTIM_SVN_INFO 3 QUTIM_SVN_REVISION)
    else( REVISION_ENTRY )
	find_path(REVISION_FILE ".revision" "${CMAKE_CURRENT_SOURCE_DIR}")
	if( REVISION_FILE )
	    file(STRINGS "${REVISION_FILE}/.revision"
		QUTIM_SVN_INFO
		LIMIT_COUNT 2)
	    list(GET QUTIM_SVN_INFO 0 QUTIM_SVN_REVISION)
	else( REVISION_FILE )
	    set( QUTIM_SVN_REVISION 0 )
	endif( REVISION_FILE )
    endif( REVISION_ENTRY )
else(NOT REVISION )
    set( QUTIM_SVN_REVISION ${REVISION} )
endif(NOT REVISION )

message( "Current revision: ${QUTIM_SVN_REVISION}" )

ADD_DEFINITIONS( -DQUTIM_SVN_REVISION=${QUTIM_SVN_REVISION} )



SET (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET (CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LIST (APPEND CMAKE_MODULE_PATH "cmake")
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
SET (QT_USE_QTNETWORK true)
SET (QT_USE_QTXML true)
SET (QT_USE_QTSCRIPT true)
SET (QT_USE_QTWEBKIT true)

SET (QUTIM_LIBS "")

IF( Phonon )
	SET (PHONON_REQUIRED)
	include( FindPhonon )
	IF(PHONON_FOUND)
		ADD_DEFINITIONS ( -DHAVE_PHONON )
		LIST(APPEND QUTIM_LIBS ${PHONON_LIBS})
	ENDIF(PHONON_FOUND)
ENDIF( Phonon )


SET (QUTIM_PLUGINS_DEST "lib/qutim/")
FIND_PACKAGE (Qt4 REQUIRED)
FIND_PACKAGE (QCA2 REQUIRED)

INCLUDE (UseQt4)
INCLUDE (MacroEnsureVersion)
INCLUDE_DIRECTORIES (${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	${QCA2_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	.
		src
		src/3rdparty/qtsolutions
		src/plugins
		src/idle
		include
		sdk02
		sdk02/include
	)

macro_ensure_version( "4.5.0" ${QTVERSION} Qt4_HAS_4_5 )

IF (UNIX)
	include( FindPkgConfig )
	pkg_check_modules (XSS xscrnsaver)
	IF( XSS_FOUND )
		ADD_DEFINITIONS( -DHAVE_XSS )
		FIND_LIBRARY( XSS_LIB NAMES Xss PATHS ${XSS_LIBDIR} )
		LIST( APPEND QUTIM_LIBS ${XSS_LIB} )
		INCLUDE_DIRECTORIES( ${XSS_INCLUDEDIR} )
	ELSE( XSS_FOUND )
		message(STATUS "Warning: libxss not found, idle detection won't be accurate")
	ENDIF( XSS_FOUND )
ENDIF (UNIX)


SET (HEADERS
	sdk02/include/qutim/plugininterface.h
	sdk02/include/qutim/protocolinterface.h
	solutions/qtcustomwidget.h
	solutions/qtcustomwidget_p.h
	src/3rdparty/qtsolutions/qtlocalpeer.h
	src/aboutinfo.h
	src/abstractlayer.h
	src/accountmanagement.h
	src/addaccountwizard.h
	src/console.h
	src/globalsettings/abstractglobalsettings.h
	src/globalsettings/globalproxysettings.h
	src/guisettingswindow.h
	src/iconmanager.h
	src/idle/idle.h
	src/logindialog.h
	src/mainsettings.h
	src/pluginsettings.h
	src/pluginsystem.h
	src/profilelogindialog.h
	src/qutim.h
	src/qutimsettings.h
	src/cryptoserviceimpl.h
	src/modulemanagerimpl.h
	)
if( Qt4_HAS_4_5 )
	LIST( APPEND HEADERS src/proxyfactory.h )
endif( Qt4_HAS_4_5 )

SET (SOURCES 
	main.cpp
	solutions/qtcustomwidget.cpp
	src/3rdparty/qtsolutions/qtlocalpeer.cpp
	src/aboutinfo.cpp
	src/abstractcontextlayer.cpp
	src/abstractlayer.cpp
	src/accountmanagement.cpp
	src/addaccountwizard.cpp
	src/console.cpp
	src/globalsettings/abstractglobalsettings.cpp
	src/globalsettings/globalproxysettings.cpp
	src/guisettingswindow.cpp
	src/iconmanager.cpp
	src/idle/idle.cpp
	src/idle/idle_mac.cpp
	src/idle/idle_win.cpp
	src/idle/idle_x11.cpp
	src/logindialog.cpp
	src/mainsettings.cpp
	src/pluginsettings.cpp
	src/pluginsystem.cpp
	src/profilelogindialog.cpp
	src/proxyfactory.cpp
	src/qutim.cpp
	src/qutimsettings.cpp
	src/cryptoserviceimpl.cpp
	src/modulemanagerimpl.cpp
	)

SET (RESOURCES
	qutim.qrc
	)

SET (FORMS
	src/aboutinfo.ui
	src/accountmanagement.ui
	src/console.ui
	src/globalsettings/globalproxysettings.ui
	src/guisettingswindow.ui
	src/logindialog.ui
	src/mainsettings.ui
	src/pluginsettings.ui
	src/profilelogindialog.ui
	src/qutim.ui
	src/qutimsettings.ui
	)

# Enum elements in corelayers directory
FILE( GLOB CORELAYERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/*" )
LIST( REMOVE_ITEM CORELAYERS .svn _svn )

#FixMe:
# This removes configbackend regardles of command line arguments
# Should be removed when configbackend is fixed.
LIST( REMOVE_ITEM CORELAYERS configbackend )

FOREACH( LAYER ${CORELAYERS} )
    IF( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}" )
        STRING( TOUPPER ${LAYER} LAYER_NAME)
        IF( NOT DEFINED "NO_${LAYER_NAME}" )
            MESSAGE( "+ layer ${LAYER_NAME} added to build" )
            FILE( GLOB_RECURSE LAYER_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.h")
            FILE( GLOB_RECURSE LAYER_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.cpp")
            FILE( GLOB_RECURSE LAYER_FORMS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.ui")
            LIST( APPEND HEADERS ${LAYER_HEADERS} )
            LIST( APPEND SOURCES ${LAYER_SOURCES} )
            LIST( APPEND FORMS ${LAYER_FORMS} )
        ELSE( NOT DEFINED "NO_${LAYER_NAME}" )
            MESSAGE( "- layer ${LAYER_NAME} will be skipped" )
            ADD_DEFINITIONS( "-DNO_CORE_${LAYER_NAME}" )
        ENDIF( NOT DEFINED "NO_${LAYER_NAME}" )
    ENDIF( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}" )
ENDFOREACH( LAYER )

#TODO fix in future
#if (MINGW)
#exec_program(windres
#ARGS "-i ${CMAKE_CURRENT_SOURCE_DIR}/qutim.rc -o qutim_res.o ")
#LIST (APPEND SOURCES qutim_res.o)
#else(MINGW)
#LIST (APPEND SOURCES qutim.rc)
#endif(MINGW)


set( QUTIM_DO_NOT_FIND "true" )
include( FindQutIM )

find_library(QUTIM_LIBRARY NAMES qutim PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libqutim)
# add_subdirectory(src/corelayers)

QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

# comment for easy development on os X
#if( APPLE )
#	SET (MACOSX_BUNDLE_ICON_FILE icons/qutim.icns)
#	ADD_EXECUTABLE (qutim MACOSX_BUNDLE ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})
#else( APPLE )
#	ADD_EXECUTABLE (qutim WIN32 ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})
#endif( APPLE )

ADD_EXECUTABLE (qutim ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})

TARGET_LINK_LIBRARIES (qutim
	${QT_LIBRARIES}
	${QCA2_LIBRARIES}
	${QUTIM_LIBS}
	${QUTIM_LIBRARY}
	)


SET (module_install_dir "${CMAKE_ROOT}/Modules")

SET (CMAKE_MODULES
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindQutIM.cmake"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/qutimuic.cmake"
)
SET (DEV_HEADERS
#TODO
)
INSTALL (FILES ${CMAKE_MODULES} DESTINATION ${module_install_dir})
INSTALL (FILES ${DEV_HEADERS} DESTINATION "include/qutim")
INSTALL (TARGETS qutim DESTINATION "bin")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/qutim.desktop" DESTINATION "share/applications")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/qutim_64.png" DESTINATION "share/icons/hicolor/64x64/apps" RENAME "qutim.png")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/qutim.xpm" DESTINATION "share/pixmaps")
