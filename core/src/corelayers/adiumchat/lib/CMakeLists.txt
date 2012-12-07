set(CMAKE_ADIUMCHAT_VERSION_MAJOR    1  CACHE INT "Major qutIM-AdiumChat version number" FORCE)
set(CMAKE_ADIUMCHAT_VERSION_MINOR    0  CACHE INT "Minor qutIM-AdiumChat version number" FORCE)
set(CMAKE_ADIUMCHAT_VERSION_PATCH    0  CACHE INT "Release qutIM-AdiumChat version number" FORCE)
set(CMAKE_ADIUMCHAT_VERSION_STRING "${CMAKE_ADIUMCHAT_VERSION_MAJOR}.${CMAKE_ADIUMCHAT_VERSION_MINOR}.${CMAKE_ADIUMCHAT_VERSION_PATCH}" CACHE STRING "qutIM-AdiumChat version string" FORCE)

file( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
file( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB FORMS "${CMAKE_CURRENT_SOURCE_DIR}/*.ui" )

set( ADIUMCHAT_TYPE "SHARED" )
if( ADIUMCHAT_BUILD_TYPE STREQUAL "STATIC" )
    set( ADIUMCHAT_TYPE "STATIC" )
    message( STATUS "libqutim-adiumchat will be linked staticaly" )
endif()
include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

qutim_wrap_cpp(MOC_SRC ${HDR})
qt4_wrap_ui(UIS_H ${FORMS})

add_library(qutim-adiumchat ${ADIUMCHAT_TYPE} ${SRC} ${MOC_SRC} ${UIS_H} ${HDR})

qutim_target_link_libraries(qutim-adiumchat
    ${QT_LIBRARIES}
    ${QUTIM_LIBRARY}
    )


set_target_properties(qutim-adiumchat PROPERTIES
    VERSION ${CMAKE_ADIUMCHAT_VERSION_STRING}
    SOVERSION ${CMAKE_ADIUMCHAT_VERSION_MAJOR}
    DEFINE_SYMBOL ADIUMCHAT_LIBRARY
)

install( TARGETS qutim-adiumchat 
	RUNTIME DESTINATION bin
		COMPONENT Core
		LIBRARY DESTINATION lib${LIB_SUFFIX}
		COMPONENT Core
		ARCHIVE DESTINATION lib${LIB_SUFFIX}
		COMPONENT CoreDevel
	BUNDLE DESTINATION library
)
