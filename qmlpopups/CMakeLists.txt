CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( QMLPopups )

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim sources")
if(QUTIM_PATH)
    set(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
    set(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
    set(QUTIM_CMAKE_PLUGIN ${QUTIM_PATH}/cmake/QutimPlugin.cmake)
else(QUTIM_PATH)
    set(QUTIM_CMAKE_PLUGIN ${CMAKE_ROOT}/Modules/QutimPlugin.cmake)
endif(QUTIM_PATH)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTSCRIPT TRUE )
SET( QT_USE_QTDECLARATIVE TRUE )
find_package( Qt4 REQUIRED )

include( ${QUTIM_CMAKE_PLUGIN} )

# Evil hack for Qt Declarative
#evil hack for qt-declarative
#find_library(QT_DECLARATIVE NAMES QtDeclarative PATHS ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)


ADD_QUTIMPLUGIN_EXT2( "qmlpopups" "src" "" ${QT_DECLARATIVE})

 
