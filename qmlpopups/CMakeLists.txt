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
# Evil hack for Qt Declarative
_QT4_ADJUST_LIB_VARS(QTDECLARATIVE)
# FIND_LIBRARY( GNUTLS_LIBRARIES gnutls)

include( ${QUTIM_CMAKE_PLUGIN} )
ADD_QUTIMPLUGIN( "qmlpopups" "src" "" )
 
