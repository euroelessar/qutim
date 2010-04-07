CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( ASpeller  )

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")
if(QUTIM_PATH)
    set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
else(QUTIM_PATH)
	set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

find_package( ASPELL REQUIRED )

include_directories( ${ASPELL_INCLUDE_DIR} )

include( ${QUTIM_CMAKE_PLUGIN} )

ADD_QUTIMPLUGIN_EXT2( "aspeller" "src" "" ${ASPELL_LIBRARIES} )
