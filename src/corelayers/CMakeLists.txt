file( GLOB qutim_core_extensions RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*" )

find_package( QutIM REQUIRED )

foreach( extension ${qutim_core_extensions} )
    if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${extension}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${extension}/CMakeLists.txt" )
		add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${extension}/" "${CMAKE_CURRENT_BINARY_DIR}/${extension}")
    endif( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${extension}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${extension}/CMakeLists.txt" )
endforeach( extension )
