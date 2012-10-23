ADD_DEFINITIONS( -DDESKTOPDIR="${CMAKE_INSTALL_PREFIX}/share/applications" )
FIND_PACKAGE(PkgConfig)
pkg_check_modules(MMENU libmessaging-menu-qt>=12.10)
if(NOT MMENU_FOUND)
    pkg_check_modules(INDICATE indicate>=0.2.1)
    pkg_check_modules(INDICATEQT indicate-qt>=0.2.2)
    if( NOT INDICATEQT_FOUND )
        message( "IndicateQt not found. Can't build indicator" )
        return()
    endif()
else()
    ADD_DEFINITIONS( -DQUANTAL=1 )
endif()

INCLUDE_DIRECTORIES(
	${INDICATE_INCLUDE_DIRS}
	${INDICATEQT_INCLUDE_DIRS}
    ${MMENU_INCLUDE_DIRS}
	)
LINK_DIRECTORIES(
	${INDICATE_LIBRARY_DIRS}
	${INDICATEQT_LIBRARY_DIRS}
    ${MMENU_LIBRARY_DIRS}
	)

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qutim ${CMAKE_INSTALL_PREFIX}/share/applications/qutim.desktop)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/qutim DESTINATION
	"share/indicators/messages/applications"
	)
qutim_add_plugin( indicator
	DISPLAY_NAME "Indicator"
	DESCRIPTION "Ubuntu Indicator applet integration"
    LINK_LIBRARIES ${INDICATE_LIBRARIES} ${INDICATEQT_LIBRARIES} ${MMENU_LIBRARIES}
)
