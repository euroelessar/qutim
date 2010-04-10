CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( MRIM )

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")

if(QUTIM_PATH)
    if(NOT QUTIM_INCLUDE_DIRS)
        set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    endif(NOT QUTIM_INCLUDE_DIRS)
    if(NOT QUTIM_LIBRARY_DIR)
        set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    endif(NOT QUTIM_LIBRARY_DIR)
    if(NOT QUTIM_CMAKE_PLUGIN)
        set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
    endif(NOT QUTIM_CMAKE_PLUGIN)
else(QUTIM_PATH)
    set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

include( "${QUTIM_CMAKE_PLUGIN}" )
SET( QT_USE_QTNETWORK TRUE )

if(NO_RTF_SUPPORT)
    ADD_DEFINITIONS("-DNO_RTF_SUPPORT")
endif(NO_RTF_SUPPORT)

ADD_QUTIMPLUGIN( "mrim" "src")

INCLUDE_DIRECTORIES( src/base
                     src/3rdparty )
