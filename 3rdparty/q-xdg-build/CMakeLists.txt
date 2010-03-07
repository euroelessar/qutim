project(q-xdg)
cmake_minimum_required(VERSION 2.6)
set(XDG_LIB_VERSION "0.1")

message("-- Checking for Qt 4...")
find_package(Qt4 REQUIRED)

set(QT_USE_QTCORE TRUE)
set(QT_USE_QTGUI TRUE)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(QXDG_SOURCES
    ../q-xdg/src/xdgenvironment.cpp
    ../q-xdg/src/xdgicontheme.cpp
    ../q-xdg/src/xdgiconmanager.cpp
    ../q-xdg/src/xdgthemechooser.cpp
    ../q-xdg/src/xdgicon.cpp
    ../q-xdg/src/xdgiconengine.cpp
)

set(QXDG_HEADERS
    ../q-xdg/src/xdg.h
    ../q-xdg/src/xdgexport.h
    ../q-xdg/src/xdgenvironment.h
    ../q-xdg/src/xdgicontheme.h
    ../q-xdg/src/xdgiconmanager.h
    ../q-xdg/src/xdgthemechooser.h
    ../q-xdg/src/xdgicon.h
)

set(QXDG_PRIVATE_HEADERS
    ../q-xdg/src/xdgicontheme_p.h
    ../q-xdg/src/xdgiconmanager_p.h
    ../q-xdg/src/xdgiconengine_p.h
)

qt4_automoc(${QXDG_SOURCES} ${TEST_SOURCES})
add_library(q-xdg STATIC ${QXDG_SOURCES} ${QXDG_HEADERS} ${QXDG_PRIVATE_HEADERS})
set_target_properties(q-xdg PROPERTIES COMPILE_FLAGS "-DXDG_STATIC")
target_link_libraries(q-xdg ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set_target_properties(q-xdg PROPERTIES VERSION ${XDG_LIB_VERSION} SOVERSION "0")
