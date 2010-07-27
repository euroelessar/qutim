CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( PhononSound )

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")
if(QUTIM_PATH)
    set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
else(QUTIM_PATH)
    set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

set(PHONON_REQUIRED)
include( FindPhonon )

include( ${QUTIM_CMAKE_PLUGIN} )
ADD_QUTIMPLUGIN_EXT2( "phononsound" "src" "" ${PHONON_LIBS})
