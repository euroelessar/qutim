# libqutim project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libqutim )

# Search for source and headers in source directory (non-recursive)
FILE( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
FILE( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

# Some utils, may be deprecated in 0.4 release
FILE( GLOB SRC_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp" )
FILE( GLOB HDR_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h" )

LIST( APPEND SRC ${SRC_UTILS} )
LIST( APPEND HDR ${HDR_UTILS} )

# Add K8JSON
option( SYSTEM_K8JSON "Use system k8json" off )
add_definitions( -DK8JSON_INCLUDE_GENERATOR -DK8JSON_INCLUDE_COMPLEX_GENERATOR )
if( SYSTEM_K8JSON )
	find_package( PkgConfig )
	pkg_check_modules( K8JSON REQUIRED k8json )
	add_definitions( -DK8JSON_LIB_DLL )
	include_directories( ${K8JSON_INCLUDE_DIRS} )
	list( APPEND EXTRA_LIBS ${K8JSON_LIBRARIES} )
else()
	list( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.cpp" )
	include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty" )
endif()

# Add qtdwm code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.h" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm_p.h" )

# Add Qxt's options implementation
set( QXT_FIND_COMPONENTS QxtCore )
find_package( Qxt )
message("QxtCore: ${QXT_FOUND_QXTCORE}")
if( QXT_FOUND_QXTCORE )
	list( APPEND EXTRA_LIBS ${QXT_QXTCORE_LIB_RELEASE} )
	include_directories( ${QXT_QXTCORE_INCLUDE_DIR} )
else()
	add_definitions( -DQXT_STATIC -DNO_SYSTEM_QXT )
	list( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qxt/qxtglobal.h" )
	list( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qxt/qxtcommandoptions.h" )
	list( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qxt/qxtcommandoptions.cpp" )
endif()

# Add QtLocalPeer
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlocalpeer.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlockedfile.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlocalpeer.h" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlockedfile.h" )
if( WIN32 )
    LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlockedfile_win.cpp" )
else()
    LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtsolutions/qtlockedfile_unix.cpp" )
endif()

# Add SlidingStackedWidget
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.h" )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.h" )

# Add flowlayout code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.h" )

# MinGW special options
# Not sure if those applicable for linux
if(MINGW)
    ADD_DEFINITIONS( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
    LIST( APPEND COMPILE_FLAGS "-mthreads" )
endif(MINGW)

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
        utils
	../3rdparty/k8json/
	../3rdparty/jdns/
	../3rdparty/qtdwm/
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook
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

ADD_LIBRARY(libqutim ${LIBQUTIM_TYPE} ${SRC} ${MOC_SRC} ${HDR} ${SRC_MM})

set_target_properties(libqutim PROPERTIES
	PREFIX ""
	VERSION ${CMAKE_QUTIM_VERSION_STRING}
	SOVERSION ${CMAKE_QUTIM_VERSION_MAJOR}
	DEFINE_SYMBOL LIBQUTIM_LIBRARY
)

if(MSVC)
	get_target_property( libqutim_flags libqutim COMPILE_FLAGS )
	set( libqutim_flags "${libqutim_flags} -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-" )
	set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "${libqutim_flags}" )
endif()
#if( CMAKE_COMPILER_IS_GNUCXX )
#	set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}" )
#endif( CMAKE_COMPILER_IS_GNUCXX )

file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/qutim" )
file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook" )
configure_file( version.h.cmake "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook/libqutim_version.h" )
file( GLOB_RECURSE QUTIM_INCLUDE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
foreach( header ${QUTIM_INCLUDE_HEADERS} )
        if( NOT ${header} MATCHES ".*_p\\.h" )
                LIST(APPEND QUTIM_INSTALL_HEADERS ${header})
    endif( NOT ${header} MATCHES ".*_p\\.h" )
endforeach( header )

list( APPEND QUTIM_INSTALL_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook/libqutim_version.h" )
set( QUTIM_LIBRARY libqutim CACHE INTERNAL "" )
set( QUTIM_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE INTERNAL "" )
QUTIM_GENERATE_INCLUDES("qutim" "${QUTIM_INSTALL_HEADERS}")
QUTIM_GENERATE_INCLUDES("qutim_hook" "${CMAKE_CURRENT_SOURCE_DIR}/libqutim_global.h")

IF(APPLE)
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(CORE_SERVICES_LIBRARY CoreServices )
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	MARK_AS_ADVANCED (CARBON_LIBRARY
		CORE_SERVICES_LIBRARY
		COCOA_LIBRARY
	)
	LIST(APPEND EXTRA_LIBS ${CARBON_LIBRARY} ${APP_SERVICES_LIBRARY} ${COCOA_LIBRARY})
ENDIF (APPLE)

IF(WIN32)
	LIST(APPEND EXTRA_LIBS ws2_32)
ENDIF()


# Link with Qt
qutim_target_link_libraries( libqutim ${QT_LIBRARIES} ${X11_LIBRARIES} ${EXTRA_LIBS})

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

option( QUTIM_GENERATE_DOCS "Generate documentation for libqutim" on )

if( DOXYGEN_FOUND AND QUTIM_GENERATE_DOCS )
    add_subdirectory(doc)
    set(DOC_TARGET "doc")
    configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(doc/DoxygenLayout.xml.in ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml @ONLY)
    add_custom_target(${DOC_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMENT "Building documentation"
        SOURCE doc/Doxyfile.in doc/DoxygenLayout.xml)

	install( DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}/doc/html
		${CMAKE_CURRENT_BINARY_DIR}/doc/qch
		DESTINATION share/qutim/doc
		COMPONENT CoreDevel
	)

    add_dependencies(libqutim ${DOC_TARGET})
endif()

