list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( LibAttica )
if( NOT LIBATTICA_FOUND )
	message( "LibAttica is not found. Can't build package manager" )
	return()
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/quasar" )
file(GLOB QUASAR_INCLUDE_HEADERS RELATIVE "" "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quasar/lib/*.h")
foreach(header ${QUASAR_INCLUDE_HEADERS})
	GET_FILENAME_COMPONENT(_basename ${header} NAME)
	if(NOT ${header} MATCHES ".*_p\\.h")
		if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/quasar/${_basename}")
			file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/quasar/${_basename}"
		"#include \"${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quasar/lib/${_basename}\"
	"
			)
		endif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/quasar/${_basename}")
	LIST(APPEND JREEN_INSTALL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quasar/lib/${header}")
	endif(NOT ${header} MATCHES ".*_p\\.h")
endforeach(header)


include_directories( ${LIBATTICA_INCLUDE_DIR}
	"${CMAKE_CURRENT_BINARY_DIR}/include"
)

qutim_add_plugin( package_manager
	DISPLAY_NAME "Package Manager"
	DESCRIPTION "Provides possibility to Get Hot New Stuff from open community"
	LINK_LIBRARIES ${LIBATTICA_LIBRARIES} quasar
)

if (${PACKAGE_MANAGER})
	add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quasar" )
endif()
