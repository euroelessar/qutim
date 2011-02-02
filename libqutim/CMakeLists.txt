# libqutim project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libqutim )
SET( QUTIM_VERSION 0.2.60.0 )

# Search for source and headers in source directory (non-recursive)
FILE( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
FILE( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB SRC_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp" )
FILE( GLOB HDR_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.h" )

LIST( APPEND SRC ${SRC_GAME} )
LIST( APPEND HDR ${HDR_GAME} )

# Add JDns sources
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/jdns.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/jdns_mdnsd.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/jdns_packet.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/jdns_sys.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/jdns_util.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/qjdns_sock.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/qjdns.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/qjdns.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/qjdns_p.h" )

# Add K8JSON code
ADD_DEFINITIONS( -DK8JSON_INCLUDE_GENERATOR -DK8JSON_INCLUDE_COMPLEX_GENERATOR )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.cpp" )
# LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.h" )

# Add qtdwm code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.h" )

# Add SlidingStackedWidget
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.h" )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.h" )

# Add flowlayout code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.h" )

# Add itemdelegate
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/itemdelegate/itemdelegate.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/itemdelegate/itemdelegate.h" )

# Add avatarfilter
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatarfilter.cpp" )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatariconengine.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatarfilter.h" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatariconengine_p.h" )

# MinGW special options
# Not sure if those applicable for linux
if(MINGW)
    ADD_DEFINITIONS( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
    LIST( APPEND COMPILE_FLAGS "-mthreads" )
endif(MINGW)

# Require QT 4.6
SET( QT_MIN_VERSION "4.6.0" )

# Set QT modules
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTGUI FALSE )

# Search for QT4
FIND_PACKAGE( Qt4 REQUIRED )

option(WITH_DOXYGEN "" ON)

if(WITH_DOXYGEN)
    message("-- Checking for Doxygen...")
    find_package(Doxygen)
endif()

# Include QT4
INCLUDE( ${QT_USE_FILE} )

# Add include directories
INCLUDE_DIRECTORIES(
	.
	..
	../sdk02/
	../3rdparty/k8json/
	../3rdparty/jdns/
	../3rdparty/qtdwm/
	../3rdparty/qticonloader/
	${CMAKE_CURRENT_BINARY_DIR}
)

# Generate moc files
QUTIM_WRAP_CPP( MOC_SRC ${HDR})
# Add JDNS using separate cmake project
#ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/jdns/" )
# Add Aero theme integration for Windows Vista and later
# ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtdwm/")

# This project will generate library

SET(LIBQUTIM_TYPE "SHARED")
IF(LIBQUTIM_BUILD_TYPE STREQUAL "STATIC")
	SET(LIBQUTIM_TYPE "STATIC")
	MESSAGE(STATUS "Libqutim will be linked staticaly")
ENDIF()

ADD_LIBRARY(libqutim ${LIBQUTIM_TYPE} ${SRC} ${MOC_SRC} ${HDR} )

#if( CMAKE_COMPILER_IS_GNUCXX )
#	set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}" )
#endif( CMAKE_COMPILER_IS_GNUCXX )

file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/qutim" )
file( GLOB_RECURSE QUTIM_INCLUDE_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
foreach( header ${QUTIM_INCLUDE_HEADERS} )
    if( NOT ${header} MATCHES ".*_p\\.h" )
		if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}" )
			file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}"
		"#include \"${CMAKE_CURRENT_SOURCE_DIR}/${header}\"
	"
			)
		endif( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}" )
	LIST(APPEND QUTIM_INSTALL_HEADERS ${header})
    endif( NOT ${header} MATCHES ".*_p\\.h" )
endforeach( header )
set( QUTIM_LIBRARIES libqutim CACHE INTERNAL "" )
set( QUTIM_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE INTERNAL "" )

# Set tagret properties
SET_TARGET_PROPERTIES ( libqutim PROPERTIES PREFIX "")

IF(APPLE)
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(CORE_SERVICES_LIBRARY CoreServices )
	MARK_AS_ADVANCED (CARBON_LIBRARY
					 CORE_SERVICES_LIBRARY)
	LIST(APPEND EXTRA_LIBS ${CARBON_LIBRARY} ${APP_SERVICES_LIBRARY})
ENDIF (APPLE)

IF(WIN32)
	LIST(APPEND EXTRA_LIBS ws2_32)
ENDIF()


# Link with QT
qutim_target_link_libraries( libqutim ${QT_LIBRARIES} ${EXTRA_LIBS})

install( TARGETS libqutim 
	RUNTIME DESTINATION bin
		COMPONENT Core
		LIBRARY DESTINATION lib${LIB_SUFFIX}
		COMPONENT Core
		ARCHIVE DESTINATION lib${LIB_SUFFIX}
		COMPONENT CoreDevel
	BUNDLE DESTINATION library
)

INSTALL(FILES ${QUTIM_INSTALL_HEADERS}
	DESTINATION include/qutim
		COMPONENT CoreDevel
)

if(DOXYGEN_FOUND)
    add_subdirectory(doc)
    set(DOC_TARGET "doc")
    configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(doc/DoxygenLayout.xml.in ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml @ONLY)
    add_custom_target(${DOC_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMENT "Building documentation"
        SOURCE doc/Doxyfile.in doc/DoxygenLayout.xml)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html ${CMAKE_CURRENT_BINARY_DIR}/doc/qch DESTINATION share/qutim/doc COMPONENT CoreDevel)
    add_dependencies(libqutim ${DOC_TARGET})
endif()

