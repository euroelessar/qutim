CMAKE_MINIMUM_REQUIRED (VERSION 2.6 FATAL_ERROR)
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT( qutim )
SET(QT_MIN_VERSION "4.6.0")

ADD_DEFINITIONS ( -DLIBQUTIM_LIBRARY )

if( UNIX )
	if( BSD )
		SET( CMAKE_THREAD_LIBS -pthread )
		SET( CMAKE_USE_PTHREADS ON )
		SET( CMAKE_EXE_LINKER_FLAGS -pthread )
	endif( BSD )
endif( UNIX )

SET (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET (CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LIST (APPEND CMAKE_MODULE_PATH "cmake")
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
SET (QT_USE_QTNETWORK true)
SET (QT_USE_QTXML true)
SET (QT_USE_QTSCRIPT true)
SET (QT_USE_QTWEBKIT true)
SET (QT_USE_QTMULTIMEDIA true)
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

INCLUDE (UseQt4)
INCLUDE (MacroEnsureVersion)
INCLUDE (QutimPlugin)

## hack for cmake 2.8.0 that does not support QtMultimedia
if( NOT QT_QTMULTIMEDIA_FOUND )
	FIND_PATH( QT_QTMULTIMEDIA_INCLUDE_DIR QtMultimedia
               PATHS
               ${QT_HEADERS_DIR}/QtMultimedia
               ${QT_LIBRARY_DIR}/QtMultimedia.framework/Headers
               NO_DEFAULT_PATH
       )
	## Only release library is being searched for.
	FIND_LIBRARY( QT_QTMULTIMEDIA_LIBRARY
                  NAMES QtMultimedia${QT_LIBINFIX} QtMultimedia${QT_LIBINFIX}4
                  PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH
       )
	if( QT_QTMULTIMEDIA_INCLUDE_DIR AND QT_QTMULTIMEDIA_LIBRARY )
		set( QT_QTMULTIMEDIA_FOUND 1 )
	endif( QT_QTMULTIMEDIA_INCLUDE_DIR AND QT_QTMULTIMEDIA_LIBRARY )
endif( NOT QT_QTMULTIMEDIA_FOUND )
## hack end

if( ${QT_QTMULTIMEDIA_FOUND} )
	include_directories( ${QT_QTMULTIMEDIA_INCLUDE_DIR} )
else( ${QT_QTMULTIMEDIA_FOUND} )
	set( QT_QTMULTIMEDIA_LIBRARY "" )
endif( ${QT_QTMULTIMEDIA_FOUND} )

INCLUDE_DIRECTORIES (${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	.
		src
		3rdparty/q-xdg
		3rdparty/flowlayout
		3rdparty/actionbox
		3rdparty/slidingstackedwidget
		3rdparty/itemdelegate
		3rdparty/avatarfilter
		include
	)


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

if( WIN32 )
	list(APPEND QUTIM_LIBS secur32 )
endif( WIN32 )

file( GLOB HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" )
file( GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
file( GLOB FORMS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui" )
list( APPEND SOURCES "main.cpp" )

SET (RESOURCES
	qutim.qrc
	)

# Enum elements in corelayers directory
FILE( GLOB CORELAYERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/*" )
LIST( REMOVE_ITEM CORELAYERS .svn _svn )

FOREACH( LAYER ${CORELAYERS} )
    IF( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}" )
        STRING( TOUPPER ${LAYER} LAYER_NAME)
        option( ${LAYER_NAME} "" ON )
        IF( ${LAYER_NAME} )
            MESSAGE( "+ layer ${LAYER_NAME} added to build" )
            FILE( GLOB_RECURSE LAYER_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.h")
            FILE( GLOB_RECURSE LAYER_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.cpp")
            FILE( GLOB_RECURSE LAYER_FORMS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}/*.ui")
            LIST( APPEND HEADERS ${LAYER_HEADERS} )
            LIST( APPEND SOURCES ${LAYER_SOURCES} )
            LIST( APPEND FORMS ${LAYER_FORMS} )
        ELSE( ${LAYER_NAME} )
            MESSAGE( "- layer ${LAYER_NAME} will be skipped" )
            ADD_DEFINITIONS( "-DNO_CORE_${LAYER_NAME}" )
        ENDIF( ${LAYER_NAME} )
    ENDIF( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/${LAYER}" )
ENDFOREACH( LAYER )

#TODO make it automaticaly
IF (${QUTIM_MOBILE_UI})
	ADD_DEFINITIONS( -DQUTIM_MOBILE_UI )
	MESSAGE( ">> qutIM will be compiled with mobile ui" )
ENDIF (${QUTIM_MOBILE_UI})
IF (${QUTIM_SINGLE_PROFILE})
	ADD_DEFINITIONS( -DQUTIM_SINGLE_PROFILE )
	MESSAGE( ">> qutIM will be compiled without multiply profiles support" )
ENDIF (${QUTIM_SINGLE_PROFILE})

# Add SlidingStackedWidget
LIST( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/slidingstackedwidget/slidingstackedwidget.cpp" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/slidingstackedwidget/slidingstackedwidget.h" )

# Add flowlayout code
LIST( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/flowlayout/flowlayout.cpp" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/flowlayout/flowlayout.h" )

# Add actionbox code
LIST( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/actionbox/actionbox.cpp" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/actionbox/actionbox_p.h" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/actionbox/actionbox.h" )

# Add itemdelegate
LIST( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/itemdelegate/itemdelegate.cpp" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/itemdelegate/itemdelegate.h" )

# Add avatarfilter
LIST( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/avatarfilter/avatarfilter.cpp" )
LIST( APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/avatarfilter/avatarfilter.h" )

#TODO fix in future
if (MINGW)
	exec_program(windres
		ARGS "-i ${CMAKE_CURRENT_SOURCE_DIR}/qutim.rc -o ${CMAKE_CURRENT_BINARY_DIR}/qutim_res.o")
	LIST (APPEND SOURCES qutim_res.o)
else(MINGW)
	LIST (APPEND SOURCES qutim.rc)
endif(MINGW)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libqutim/" "${CMAKE_CURRENT_BINARY_DIR}/libqutim")
set( XDG_NOT_BUILD_TEST true )
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/q-xdg-build" "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/q-xdg")
REMOVE_DEFINITIONS ( -DLIBQUTIM_LIBRARY )
ADD_DEFINITIONS ( -DQUTIM_CORE )

QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE qutim.icns)
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/icons/qutim.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/qutim.icns)
ENDIF(APPLE)

ADD_EXECUTABLE (qutim ${GUI_TYPE} ${SOURCES} ${HEADERS} ${MOC_SRCS} ${UIS_H} ${RCC})

if( CMAKE_COMPILER_IS_GNUCXX )
	set_target_properties( qutim PROPERTIES COMPILE_FLAGS "-DQUTIM_CORE -DXDG_STATIC -Wall -Werror" )
	set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra" )
	if( NOT WIN32 )
		set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "-fvisibility=hidden" )
	endif( NOT WIN32 )
else( CMAKE_COMPILER_IS_GNUCXX )
	set_target_properties( qutim PROPERTIES COMPILE_FLAGS "-DQUTIM_CORE -DXDG_STATIC" )
endif( CMAKE_COMPILER_IS_GNUCXX )

TARGET_LINK_LIBRARIES (qutim
	${QT_LIBRARIES}
	${QT_QTMULTIMEDIA_LIBRARY}
	${QT_QTMAIN_LIBRARY}
	${QUTIM_LIBS}
	libqutim
	q-xdg
	)


if( LANGUAGE )
	LANGUAGE_UPDATE( core ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
endif( LANGUAGE )

SET (module_install_dir "${CMAKE_ROOT}/Modules")

SET (CMAKE_MODULES
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindQutIM.cmake"
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/qutimuic.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QutimPlugin.cmake"
)

FILE (GLOB DEV_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*.h")
FILE (GLOB DEV_HEADERS_EXCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*_p.h")
LIST (REMOVE_ITEM DEV_HEADERS ${DEV_HEADERS_EXCLUDE}) 

#TODO: Fix installation of cmake modules. Now those placed directly to /usr (absolute path)
INSTALL (FILES ${CMAKE_MODULES} DESTINATION ${module_install_dir})
INSTALL (FILES ${DEV_HEADERS} DESTINATION "include/qutim")
INSTALL (TARGETS qutim DESTINATION "bin")
INSTALL( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share" DESTINATION "${CMAKE_INSTALL_PREFIX}/" PATTERN ".svn" EXCLUDE PATTERN "_svn" EXCLUDE )
