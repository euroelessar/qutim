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
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
SET (QT_USE_QTNETWORK true)
SET (QT_USE_QTXML true)
SET (QT_USE_QTSCRIPT true)

SET (QUTIM_PLUGINS_DEST "lib/qutim/")
FIND_PACKAGE (Qt4 REQUIRED)

INCLUDE (UseQt4)
INCLUDE (MacroEnsureVersion)

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

INCLUDE_DIRECTORIES (${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${3RDPARTY_INCLUDE_DIRS}
	.
		src
		include
	)

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

#TODO fix in future
if (MINGW)
	SET(MINGW_WINDRES "mingw32-windres") #TODO add program check
	exec_program(${MINGW_WINDRES}
		ARGS "-i ${CMAKE_CURRENT_SOURCE_DIR}/qutim.rc -o ${CMAKE_CURRENT_BINARY_DIR}/qutim_res.o")
	LIST (APPEND SOURCES qutim_res.o)
else(MINGW)
	LIST (APPEND SOURCES qutim.rc)
endif(MINGW)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libqutim/" "${CMAKE_CURRENT_BINARY_DIR}/libqutim")

find_package( QutIM REQUIRED )
include_directories( ${QUTIM_INCLUDE_DIR} )

REMOVE_DEFINITIONS ( -DLIBQUTIM_LIBRARY )

set( QUTIM_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "Path to qutim sources")
set( QUTIM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE FILEPATH "Path to qutim build directory")
option( QUTIM_COPY_PLUGINS_TO_BINARY_DIR "Copy plugins to qutim build directory" OFF )
set( QUTIM_LIBRARIES libqutim )

QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCC ${RESOURCES})

# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE qutim.icns)
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/qutim.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/qutim.icns)
ENDIF(APPLE)

set( QUTIM_ADDITIONAL_SOURCES "" CACHE INTERNAL "" FORCE )
set( QUTIM_ADDITIONAL_LIBRARIES "" CACHE INTERNAL "" FORCE )
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/corelayers/" "${CMAKE_CURRENT_BINARY_DIR}/src/corelayers")
foreach( additional_path ${QUTIM_ADDITIONAL_PATHS} )
    add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/../${additional_path}/" "${CMAKE_CURRENT_BINARY_DIR}/../${additional_path}" )
endforeach( additional_path ${QUTIM_ADDITIONAL_PATHS} )

QUTIM_ADD_OPT(QUTIM_MOBILE_UI
	"User interface for mobile devices like Nokia 5800"
	"qutIM will be compiled with mobile ui"
	OFF
)
QUTIM_ADD_OPT(QUTIM_SINGLE_PROFILE
	"Single profil by default" 
	"qutIM will be compiled without multiply profiles by default"
	ON
)
SET( QUTIM_GUI_TYPE "" CACHE STRING "qutIM GUI TYPE" )
IF( QUTIM_GUI_TYPE )
	MESSAGE(STATUS "qutIM will be compiled with ${QUTIM_GUI_TYPE} GUI type")
ENDIF()

ADD_EXECUTABLE (qutim ${QUTIM_GUI_TYPE} ${SOURCES} ${HEADERS} ${MOC_SRCS} ${UIS_H} ${RCC} ${QUTIM_ADDITIONAL_SOURCES} )

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
	${QUTIM_LIBS}
	${QUTIM_ADDITIONAL_LIBRARIES}
	libqutim
	)

if( LANGUAGE )
	LANGUAGE_UPDATE( core ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
endif( LANGUAGE )

FILE(GLOB CMAKE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake")

FILE (GLOB DEV_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*.h")
FILE (GLOB DEV_HEADERS_EXCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/libqutim/*_p.h")
LIST (REMOVE_ITEM DEV_HEADERS ${DEV_HEADERS_EXCLUDE}) 

INSTALL (FILES ${CMAKE_MODULES} DESTINATION share/cmake/Modules)
INSTALL (TARGETS qutim RUNTIME DESTINATION bin)
INSTALL( DIRECTORY share/qutim DESTINATION share PATTERN ".svn" EXCLUDE PATTERN "_svn" EXCLUDE )
