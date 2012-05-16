CMAKE_MINIMUM_REQUIRED (VERSION 2.6 FATAL_ERROR)
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)
PROJECT(qutim)

set(CMAKE_QUTIM_VERSION_MAJOR    0  CACHE INT "Major qutIM version number" FORCE)
set(CMAKE_QUTIM_VERSION_MINOR    3  CACHE INT "Minor qutIM version number" FORCE)
set(CMAKE_QUTIM_VERSION_SECMINOR 1 CACHE INT "Secminor qutIM version number" FORCE)
set(CMAKE_QUTIM_VERSION_PATCH    0  CACHE INT "Release qutIM version number" FORCE)
set(CMAKE_QUTIM_VERSION_STRING "${CMAKE_QUTIM_VERSION_MAJOR}.${CMAKE_QUTIM_VERSION_MINOR}.${CMAKE_QUTIM_VERSION_SECMINOR}.${CMAKE_QUTIM_VERSION_PATCH}" CACHE STRING "qutIM version string" FORCE)

ADD_DEFINITIONS(-DLIBQUTIM_LIBRARY)

if(UNIX)
        if(BSD)
                set(CMAKE_THREAD_LIBS -pthread)
                set(CMAKE_USE_PTHREADS ON )
                set(CMAKE_EXE_LINKER_FLAGS -pthread)
        endif(BSD)
endif(UNIX)

SET (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
SET (CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
SET (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
SET (QT_USE_QTNETWORK true)
SET (QT_USE_QTXML true)
SET (QT_USE_QTSCRIPT true)
if( DECLARATIVE_UI )
	set( QT_USE_QTDECLARATIVE true )
endif()

SET(QUTIM_PLUGINS_DEST "lib/qutim/")
FIND_PACKAGE (Qt4 REQUIRED)

INCLUDE(UseQt4)
INCLUDE(MacroEnsureVersion)
INCLUDE(QutIMMacros)

#simple hack for include dirs
macro(LIST_SUBDIRECTORIES retval curdir)
	file(GLOB sub-dir RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${curdir}/*)
	set(list_of_dirs "")
	foreach(dir ${sub-dir})
		#TODO fix IS DIRECTORY macro
		if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${dir})
			message(STATUS "Found 3rdparty dir: ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${dir}")
		endif()
	endforeach()
	set(${retval} ${sub-dir})
endmacro()

LIST_SUBDIRECTORIES(3RDPARTY_INCLUDE_DIRS "3rdparty")

#Include X11, spike for freebsd
FIND_PACKAGE(X11)

INCLUDE_DIRECTORIES (.
	..
	../3rdparty
	src
	${CMAKE_CURRENT_BINARY_DIR}
	include
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}	
	${3RDPARTY_INCLUDE_DIRS}
	${X11_INCLUDE_DIR}		
	)

if( WIN32 )
	list(APPEND QUTIM_LIBS secur32 )
endif( WIN32 )

if( DECLARATIVE_UI )
	set( CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/declarative" )
	install(DIRECTORY ${CORE_SOURCE_DIR}/profilecreator 
		DESTINATION ${QUTIM_SHARE_DIR}/declarative
	)
	include_directories( ${QT_QTDECLARATIVE_INCLUDE_DIR} )
	add_definitions( -DQUTIM_DECLARATIVE_UI )
else()
	set( CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/widgets" )
endif()

file( GLOB HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CORE_SOURCE_DIR}/*.h" )
file( GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CORE_SOURCE_DIR}/*.cpp" )
file( GLOB SOURCES_MM RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CORE_SOURCE_DIR}/*.mm" )
list( APPEND SOURCE SOURCE_MM} )
file( GLOB FORMS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CORE_SOURCE_DIR}/*.ui" )
list( APPEND SOURCES "main.cpp" )

SET(RESOURCES
	qutim.qrc
	)

#TODO fix in future
if(MINGW)
	if( NOT CMAKE_RC_COMPILER )
		find_program( CMAKE_RC_COMPILER NAMES "mingw32-windres" "windres" )
	endif()

	if( NOT CMAKE_RC_COMPILER )
		message("No RC compiler (windres) found.")
		return()
	endif()

	execute_process( COMMAND "${CMAKE_RC_COMPILER}"
		"-i" "${CMAKE_CURRENT_SOURCE_DIR}/qutim.rc" "-o" "${CMAKE_CURRENT_BINARY_DIR}/qutim_res.o" )
	LIST (APPEND SOURCES qutim_res.o)
else(MINGW)
	LIST (APPEND SOURCES qutim.rc)
endif(MINGW)

add_subdirectory(libqutim)

find_package(QutIM REQUIRED)
include_directories( ${QUTIM_INCLUDE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/libqutim/include/qutim_hook" )

REMOVE_DEFINITIONS ( -DLIBQUTIM_LIBRARY )

set(QUTIM_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "Path to qutim sources")
set(QUTIM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE FILEPATH "Path to qutim build directory")
option(QUTIM_COPY_PLUGINS_TO_BINARY_DIR "Copy plugins to qutim build directory" OFF)
set(QUTIM_LIBRARY libqutim)

# For Apple set the icns file containing icons
IF(APPLE)
	# set how it shows up in the Info.plist file
	SET(MACOSX_BUNDLE_ICON_FILE qutim.icns)
	SET(MACOSX_BUNDLE_INFO_STRING "Module based instant messenger")
	SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${CMAKE_QUTIM_VERSION_STRING}")
	SET(MACOSX_BUNDLE_BUNDLE_NAME "qutIM")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "${CMAKE_QUTIM_VERSION_STRING}")
	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/qutim.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	# include the icns file in the target
	SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/qutim.icns)
	GET_FILENAME_COMPONENT(qt_lib_path ${QT_QTGUI_LIBRARY_RELEASE} PATH)
	find_path(qtmenu_nib_path NAMES "classes.nib" "info.nib"  PATHS "${QT_QTGUI_LIBRARY_RELEASE}/Resources/qt_menu.nib"
		"${qt_lib_path}/Resources/qt_menu.nib"
	)
	set(qt_menu_nib_sources "${qtmenu_nib_path}/classes.nib" "${qtmenu_nib_path}/info.nib" "${qtmenu_nib_path}/keyedobjects.nib" )
	SET_SOURCE_FILES_PROPERTIES(${qt_menu_nib_sources} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib)
	SET(SOURCES ${SOURCES} ${qt_menu_nib_sources})
ENDIF(APPLE)

IF(APPLE)
        IF(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
		set (QUTIM_BUNDLE_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/qutim.app")
		add_custom_target(qutim-bundle ALL)
		add_dependencies(qutim-bundle qutim translations)
	ELSE(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
		message (WARNING "You have to use Unix Makefiles generator to automatically bundle deploying")
		set (QUTIM_BUNDLE_LOCATION "")
	ENDIF(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
ENDIF(APPLE)

#additional resources path
set(QUTIM_ADDITIONAL_RESOURCES_PATH "" CACHE PATH "Path to additional resources")

set(QUTIM_ADDITIONAL_SOURCES "" CACHE INTERNAL "" FORCE )
set(QUTIM_ADDITIONAL_LIBRARIES "" CACHE INTERNAL "" FORCE )
set(QUTIM_ADDITIONAL_RCC ${QUTIM_ADDITIONAL_RESOURCES_PATH} CACHE INTERNAL "" FORCE )

add_subdirectory(src/corelayers)
foreach(additional_path ${QUTIM_ADDITIONAL_PATHS} )
	add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/../${additional_path}/" "${CMAKE_CURRENT_BINARY_DIR}/../${additional_path}" )
endforeach(additional_path ${QUTIM_ADDITIONAL_PATHS})
add_subdirectory(share)

message(STATUS ${QUTIM_ADDITIONAL_RCC})
list(APPEND RESOURCES ${QUTIM_ADDITIONAL_RCC})

QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

#QUTIM_ADD_OPTION(QUTIM_MOBILE_UI
#	"User interface for mobile devices like Nokia 5800"
#	"qutIM will be compiled with mobile ui"
#	OFF
#)

# TODO: Improve startup speed
if( MAEMO6 )
	INCLUDE(FindPkgConfig)
	pkg_check_modules(QDECLARATIVE_BOOSTABLE qdeclarative-boostable)
	list(APPEND QUTIM_ADDITIONAL_LIBRARIES ${QDECLARATIVE_BOOSTABLE_LIBRARIES} )
	INCLUDE_DIRECTORIES (${QDECLARATIVE_BOOSTABLE_INCLUDE_DIRS})
endif()

QUTIM_ADD_OPTION(QUTIM_SINGLE_PROFILE
	"Single profile by default" 
	"qutIM will be compiled without multiply profiles by default"
	ON
)

QUTIM_ADD_EXECUTABLE(qutim
	WIN32 MACOSX_BUNDLE
	${SOURCES}
	${HEADERS}
	${MOC_SRCS}
	${UIS_H}
	${RCC}
	${QUTIM_ADDITIONAL_SOURCES}
	)

if(MSVC)
	set(QUTIM_COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS} /W3")
	set(LIBQUTIM_COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS} /W3")
else()
	if(SYMBIAN)
		#disable stupid warnings
		set(QUTIM_COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS} -w")
		set(LIBQUTIM_COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS} -w")
	else()
		set(QUTIM_COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS} -Wall -Wextra")
		set(LIBQUTIM_COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS} -Wall -Wextra")
		if(NOT WIN32)
			set(LIBQUTIM_COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS} -fvisibility=hidden")
			if(DEVELOPER)
				set(QUTIM_COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS} -Werror")
				set(LIBQUTIM_COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS} -Werror")
			endif(DEVELOPER)
		endif(NOT WIN32)
	endif()
endif()

set(QUTIM_COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS} -DQUTIM_CORE -DXDG_STATIC")
set_target_properties(qutim PROPERTIES COMPILE_FLAGS "${QUTIM_COMPILE_FLAGS}")
set_target_properties(libqutim PROPERTIES COMPILE_FLAGS "${LIBQUTIM_COMPILE_FLAGS}")

qutim_target_link_libraries(qutim
	${QT_LIBRARIES}
	${QT_QTMAIN_LIBRARY}
	${QUTIM_LIBS}
	${QUTIM_ADDITIONAL_LIBRARIES}
	libqutim
	)

IF(APPLE AND QUTIM_BUNDLE_LOCATION)

	macro(_deploy_plugins_dir _plugin)
		set(plugin_dir "${QT_PLUGINS_DIR}/${_plugin}")
		file(GLOB plugins ${plugin_dir} "${plugin_dir}/*.dylib")
		get_filename_component(category ${plugin_dir} NAME_WE)
		get_filename_component(_abs_FILE ${plugin_dir} ABSOLUTE)
		foreach(plugin ${plugins})
			get_filename_component(_abs_PATH ${plugin} ABSOLUTE)
			get_filename_component(name ${plugin} NAME)
			#TODO remove this hack
			if(NOT ${_abs_PATH} STREQUAL ${_abs_FILE})
				add_custom_command(TARGET qutim-bundle POST_BUILD
					COMMAND ${CMAKE_COMMAND}
						ARGS -E copy
						${_abs_PATH}
						${QUTIM_BUNDLE_LOCATION}/Contents/PlugIns/${category}/${name}
				)
			endif()
		endforeach()
	endmacro()

	macro(_mac_deploy_plugin _dir _plugin)
		set(plugin "${_dir}/lib${_plugin}.dylib") #evil hack
		get_filename_component(_absfile "${QT_PLUGINS_DIR}/${plugin}" ABSOLUTE)
		if(EXISTS ${_absfile})
			message(STATUS "found ${_dir}/${_plugin} plugin.")
			add_custom_command(TARGET qutim-bundle POST_BUILD
				COMMAND ${CMAKE_COMMAND}
					ARGS -E copy
					${_absfile}
					${QUTIM_BUNDLE_LOCATION}/Contents/PlugIns/${plugin}
			)
		endif()
	endmacro()

	_mac_deploy_plugin(bearer qcorewlanbearer)
	_mac_deploy_plugin(bearer qgenericbearer)

	_mac_deploy_plugin(iconengines qsvgicon)

	_mac_deploy_plugin(imageformats qgif)
	_mac_deploy_plugin(imageformats qsvg)
	_mac_deploy_plugin(imageformats qjpeg)
	_mac_deploy_plugin(imageformats qmng)

	_mac_deploy_plugin(crypto qca-ossl)
	_mac_deploy_plugin(crypto qca-cyrus-sasl)
	_mac_deploy_plugin(crypto qca-gnupg)

	add_custom_command(TARGET qutim-bundle POST_BUILD
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/deployqutim.sh
			${QUTIM_BUNDLE_LOCATION}
		)

ENDIF(APPLE AND QUTIM_BUNDLE_LOCATION)

if(LANGUAGE)
	LANGUAGE_UPDATE( core ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
endif( LANGUAGE )

FILE(GLOB CMAKE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake")

FILE (GLOB DEV_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*.h")
FILE (GLOB DEV_HEADERS_EXCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*_p.h")
LIST(REMOVE_ITEM DEV_HEADERS ${DEV_HEADERS_EXCLUDE})

INSTALL(FILES ${CMAKE_MODULES}
	DESTINATION share/cmake/Modules
		COMPONENT CoreDevel
)
INSTALL(TARGETS qutim
	RUNTIME DESTINATION bin
		COMPONENT Core
	BUNDLE DESTINATION .
		COMPONENT Core
)

IF(UNIX AND NOT APPLE AND NOT MAEMO5)
	INSTALL(DIRECTORY share/applications share/icons share/pixmaps
		DESTINATION share
			COMPONENT Core
	)
ENDIF()

IF(MAEMO5)
	INSTALL(DIRECTORY share/applications share/icons share/pixmaps
		DESTINATION /usr/share
			COMPONENT Core
	)
ENDIF()

CPACK_ADD_COMPONENT(Core
	DISPLAY_NAME "Core"
	DESCRIPTION "TODO"
)

if(QUTIM_DEVELOPER_BUILD)
	message(STATUS "Selected developer build. Development tools will be installed")
else()
	message(STATUS "Development tools will not be installed")
endif()

qutim_add_devel_component(CoreDevel
	DISPLAY_NAME "Core Devel"
	DESCRIPTION "Common devel files"
)

#additional artwork sources
SET(QUTIM_ADDITIONAL_ART_PATH "-NOTFOUND" CACHE PATH "Path to additional artwork")
IF(QUTIM_ADDITIONAL_ART_PATH)
	QUTIM_ADD_ARTWORK_DIR(${QUTIM_ADDITIONAL_ART_PATH})
ENDIF()

option(QUTIM_INSTALL_ICON_THEME "Install built-in icon theme based on oxygen" OFF)
if(QUTIM_INSTALL_ICON_THEME)
	message(STATUS "qutIM built-in icon theme will be installed")
	qutim_add_artwork(../artwork/icons)
endif()

CPACK_ADD_COMPONENT(Core
	DISPLAY_NAME "Core"
	DESCRIPTION "TODO"
)
CPACK_ADD_COMPONENT(CoreDevel
	DISPLAY_NAME "Core Devel"
	DESCRIPTION "Common devel files"
)

#small simple hack for install Qt dlls and plugins
IF(WIN32)
		SET(QUTIM_REQUIRED_DLL_DIR "" CACHE PATH "Path to Qt dll's and plugins. Note: you must add them yourself")
		if(NOT ${QUTIM_REQUIRED_DLL_DIR} STREQUAL "")
			FILE(GLOB dlls ${QUTIM_REQUIRED_DLL_DIR}/*)
			FOREACH(QUTIM_DLL ${dlls})
					IF(IS_DIRECTORY ${QUTIM_DLL})
							INSTALL(DIRECTORY ${QUTIM_DLL}
									DESTINATION bin
											COMPONENT Qt
							)
					ELSEIF(${QUTIM_DLL} MATCHES ".\\.dll")
							INSTALL(FILES ${QUTIM_DLL}
									DESTINATION bin
											COMPONENT Qt
							)
					ENDIF()
			ENDFOREACH()
			LIST(APPEND CPACK_COMPONENTS_ALL Qt)
			CPACK_ADD_COMPONENT(Qt
					DISPLAY_NAME "Qt Libraries"
					DESCRIPTION ""
			)
		endif()
ENDIF(WIN32)

