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
IF(NOT NO_CHAT)
	SET (QT_USE_QTWEBKIT true)
	SET (QT_USE_QTUITOOLS true)
ENDIF(NOT NO_CHAT)
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
	include/qutim/plugininterface.h
	include/qutim/protocolinterface.h
	solutions/qtcustomwidget.h
	solutions/qtcustomwidget_p.h
	src/3rdparty/qtsolutions/qtlocalpeer.h
	src/aboutinfo.h
	src/abstractcontextlayer.h
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

IF(NOT NO_ANTISPAM)
	LIST (APPEND HEADERS
		src/corelayers/antispam/abstractantispamlayer.h
		src/corelayers/antispam/antispamlayersettings.h
		)
	LIST (APPEND SOURCES
		src/corelayers/antispam/abstractantispamlayer.cpp
		src/corelayers/antispam/antispamlayersettings.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/antispam/antispamlayersettings.ui
		)
ELSE(NOT NO_ANTISPAM)
	ADD_DEFINITIONS( -DNO_CORE_ANTISPAM )
ENDIF(NOT NO_ANTISPAM)
IF(NOT NO_CHAT)
	LIST (APPEND HEADERS
		src/corelayers/chat/chatemoticonmenu.h
		src/corelayers/chat/chatforms/chatwindow.h
		src/corelayers/chat/chatforms/conferencewindow.h
		src/corelayers/chat/chatlayerclass.h
		src/corelayers/chat/chatwindowstyle.h
		src/corelayers/chat/chatwindowstyleoutput.h
		src/corelayers/chat/confcontactlist.h
		src/corelayers/chat/conferenceitem.h
		src/corelayers/chat/conferenceitemmodel.h
		src/corelayers/chat/conferencetabcompletion.h
		src/corelayers/chat/generalwindow.h
		src/corelayers/chat/logscity.h
		src/corelayers/chat/movielabel.h
		src/corelayers/chat/separatechats.h
		src/corelayers/chat/settings/chatlayersettings.h
		src/corelayers/chat/tabbedchats.h
		src/corelayers/chat/tempglobalinstance.h
		)
	LIST (APPEND SOURCES
		src/corelayers/chat/chatemoticonmenu.cpp
		src/corelayers/chat/chatforms/chatwindow.cpp
		src/corelayers/chat/chatforms/conferencewindow.cpp
		src/corelayers/chat/chatlayerclass.cpp
		src/corelayers/chat/chatwindowstyle.cpp
		src/corelayers/chat/chatwindowstyleoutput.cpp
		src/corelayers/chat/confcontactlist.cpp
		src/corelayers/chat/conferenceitem.cpp
		src/corelayers/chat/conferenceitemmodel.cpp
		src/corelayers/chat/conferencetabcompletion.cpp
		src/corelayers/chat/generalwindow.cpp
		src/corelayers/chat/logscity.cpp
		src/corelayers/chat/movielabel.cpp
		src/corelayers/chat/separatechats.cpp
		src/corelayers/chat/settings/chatlayersettings.cpp
		src/corelayers/chat/tabbedchats.cpp
		src/corelayers/chat/tempglobalinstance.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/chat/chatforms/chatform.ui
		src/corelayers/chat/chatforms/conferenceform.ui
		src/corelayers/chat/settings/chatlayersettings.ui
		)
ELSE(NOT NO_CHAT)
	ADD_DEFINITIONS( -DNO_CORE_CHAT )
ENDIF(NOT NO_CHAT)
IF(NOT NO_CONTACTLIST)
	LIST (APPEND HEADERS
		src/corelayers/contactlist/contactlistitemdelegate.h
		src/corelayers/contactlist/contactlistproxymodel.h
		src/corelayers/contactlist/contactlistsettings.h
		src/corelayers/contactlist/defaultcontactlist.h
		src/corelayers/contactlist/proxymodelitem.h
		src/corelayers/contactlist/treecontactlistmodel.h
		src/corelayers/contactlist/treeitem.h
		)
	LIST (APPEND SOURCES
		src/corelayers/contactlist/contactlistitemdelegate.cpp
		src/corelayers/contactlist/contactlistproxymodel.cpp
		src/corelayers/contactlist/contactlistsettings.cpp
		src/corelayers/contactlist/defaultcontactlist.cpp
		src/corelayers/contactlist/proxymodelitem.cpp
		src/corelayers/contactlist/treecontactlistmodel.cpp
		src/corelayers/contactlist/treeitem.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/contactlist/contactlistsettings.ui
		src/corelayers/contactlist/defaultcontactlist.ui
		)
ELSE(NOT NO_CONTACTLIST)
	IF(NOT NO_CHAT)
		LIST (APPEND HEADERS src/corelayers/contactlist/contactlistitemdelegate.h)
		LIST (APPEND SOURCES src/corelayers/contactlist/contactlistitemdelegate.cpp)
	ENDIF(NOT NO_CHAT)
	ADD_DEFINITIONS( -DNO_CORE_CONTACTLIST )
ENDIF(NOT NO_CONTACTLIST)
IF(NOT NO_EMOTICONS)
	LIST (APPEND HEADERS
		src/corelayers/emoticons/abstractemoticonslayer.h
		)
	LIST (APPEND SOURCES
		src/corelayers/emoticons/abstractemoticonslayer.cpp
		)
ELSE(NOT NO_EMOTICONS)
	ADD_DEFINITIONS( -DNO_CORE_EMOTICONS )
ENDIF(NOT NO_EMOTICONS)
IF(NOT NO_HISTORY)
	LIST (APPEND HEADERS
		src/corelayers/history/historyengine.h
		src/corelayers/history/historylayer.h
		src/corelayers/history/historysettings.h
		src/corelayers/history/historywindow.h
		)
	LIST (APPEND SOURCES
		src/corelayers/history/historyengine.cpp
		src/corelayers/history/historylayer.cpp
		src/corelayers/history/historysettings.cpp
		src/corelayers/history/historywindow.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/history/historysettings.ui
		src/corelayers/history/historywindow.ui
		)
ELSE(NOT NO_HISTORY)
	ADD_DEFINITIONS( -DNO_CORE_HISTORY )
ENDIF(NOT NO_HISTORY)
IF(NOT NO_NOTIFICATION)
	LIST (APPEND HEADERS
		src/corelayers/notification/defaultnotificationlayer.h
		src/corelayers/notification/notificationslayersettings.h
		src/corelayers/notification/popuptextbrowser.h
		src/corelayers/notification/popupwindow.h
		src/corelayers/notification/soundlayersettings.h
		)
	LIST (APPEND SOURCES
		src/corelayers/notification/defaultnotificationlayer.cpp
		src/corelayers/notification/notificationslayersettings.cpp
		src/corelayers/notification/popuptextbrowser.cpp
		src/corelayers/notification/popupwindow.cpp
		src/corelayers/notification/soundlayersettings.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/notification/notificationslayersettings.ui
		src/corelayers/notification/popupwindow.ui
		src/corelayers/notification/soundlayersettings.ui
		)
ELSE(NOT NO_NOTIFICATION)
	ADD_DEFINITIONS( -DNO_CORE_NOTIFICATION )
ENDIF(NOT NO_NOTIFICATION)
IF(NOT NO_SETTINGS)
	LIST (APPEND HEADERS
		src/corelayers/settings/qsettingslayer.h
		)
	LIST (APPEND SOURCES
		src/corelayers/settings/qsettingslayer.cpp
		)
ELSE(NOT NO_SETTINGS)
	ADD_DEFINITIONS( -DNO_CORE_SETTINGS )
ENDIF(NOT NO_SETTINGS)
IF(NOT NO_SOUNDENGINE)
	LIST (APPEND HEADERS
		src/corelayers/soundengine/defaultsoundenginelayer.h
		src/corelayers/soundengine/soundenginesettings.h
		)
	LIST (APPEND SOURCES
		src/corelayers/soundengine/defaultsoundenginelayer.cpp
		src/corelayers/soundengine/soundenginesettings.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/soundengine/soundenginesettings.ui
		)
ELSE(NOT NO_SOUNDENGINE)
	ADD_DEFINITIONS( -DNO_CORE_SOUNDENGINE )
ENDIF(NOT NO_SOUNDENGINE)
IF(NOT NO_SPELLER)
	LIST (APPEND HEADERS
		src/corelayers/speller/spellerhighlighter.h
		src/corelayers/speller/spellerlayerclass.h
		)
	LIST (APPEND SOURCES
		src/corelayers/speller/spellerhighlighter.cpp
		src/corelayers/speller/spellerlayerclass.cpp
		)
ELSE(NOT NO_SPELLER)
	ADD_DEFINITIONS( -DNO_CORE_SPELLER )
ENDIF(NOT NO_SPELLER)
IF(NOT NO_STATUS)
	LIST (APPEND HEADERS
		src/corelayers/status/defaultstatuslayer.h
		src/corelayers/status/statusdialog.h
		src/corelayers/status/statuspresetcaption.h
		)
	LIST (APPEND SOURCES
		src/corelayers/status/defaultstatuslayer.cpp
		src/corelayers/status/statusdialog.cpp
		src/corelayers/status/statuspresetcaption.cpp
		)
	LIST (APPEND FORMS
		src/corelayers/status/statusdialogvisual.ui
		src/corelayers/status/statuspresetcaption.ui
		)
ELSE(NOT NO_STATUS)
	ADD_DEFINITIONS( -DNO_CORE_STATUS )
ENDIF(NOT NO_STATUS)
IF(NOT NO_TRAY)
	LIST (APPEND HEADERS
		src/corelayers/tray/defaulttraylayer.h
		src/corelayers/tray/exsystrayicon.h
		)
	LIST (APPEND SOURCES
		src/corelayers/tray/defaulttraylayer.cpp
		src/corelayers/tray/exsystrayicon.cpp
		)
ELSE(NOT NO_TRAY)
	ADD_DEFINITIONS( -DNO_CORE_TRAY )
ENDIF(NOT NO_TRAY)

IF (WIN32)
#	LIST (APPEND SOURCES
#		src/ex/exsystrayicon.cpp
#		 src/idle/idle_win.cpp
#		)
#	LIST (APPEND HEADERS
#		src/ex/exsystrayicon.h
#		)
		
if (MINGW)
exec_program(windres
ARGS "-i ${CMAKE_CURRENT_SOURCE_DIR}/qutim.rc -o qutim_res.o ")
LIST (APPEND SOURCES qutim_res.o)
else(MINGW)
LIST (APPEND SOURCES qutim.rc)
endif(MINGW)
 
ENDIF (WIN32)

#IF (APPLE)
#    LIST (APPEND SOURCES
#		src/ex/exsystrayicon.cpp
#	src/idle/idle_mac.cpp
#		)
#    LIST (APPEND HEADERS
#		src/ex/exsystrayicon.h
#		)
#	SET (MACOSX_BUNDLE_ICON_FILE icons/qutim.icns)
#
#ELSEIF (UNIX)
#	LIST (APPEND SOURCES
#		src/idle/idle_x11.cpp
#		)
#ENDIF ()


set( QUTIM_DO_NOT_FIND "true" )
include( FindQutIM )

QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

if( APPLE )
	SET (MACOSX_BUNDLE_ICON_FILE icons/qutim.icns)
	ADD_EXECUTABLE (qutim MACOSX_BUNDLE ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})
else( APPLE )
	ADD_EXECUTABLE (qutim WIN32 ${SOURCES} ${MOC_SRCS} ${UIS_H} ${RCC})
endif( APPLE )

find_library(QUTIM_LIBRARY NAMES qutim PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libqutim)

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
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/iconmanagerinterface.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/layerinterface.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/layerscity.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/plugininterface.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/protocolinterface.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/qutim/settings.h"
)
INSTALL (FILES ${CMAKE_MODULES} DESTINATION ${module_install_dir})
INSTALL (FILES ${DEV_HEADERS} DESTINATION "include/qutim")
INSTALL (TARGETS qutim DESTINATION "bin")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/qutim.desktop" DESTINATION "share/applications")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/qutim_64.png" DESTINATION "share/icons/hicolor/64x64/apps" RENAME "qutim.png")
INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/qutim.xpm" DESTINATION "share/pixmaps")
