project(q-xdg)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(QXDG_SOURCES
    src/xdgenvironment.cpp
    src/xdgicontheme.cpp
    src/xdgiconmanager.cpp
    src/xdgthemechooser.cpp
    src/xdgicon.cpp
    src/xdgiconengine.cpp
)

set(QXDG_HEADERS
    src/xdg.h    
    src/xdgexport.h
    src/xdgenvironment.h
    src/xdgicontheme.h
    src/xdgiconmanager.h
    src/xdgthemechooser.h
    src/xdgicon.h
)

set(QXDG_PRIVATE_HEADERS
    src/xdgicontheme_p.h
    src/xdgiconmanager_p.h
    src/xdgiconengine_p.h
)

qt4_automoc(${QXDG_SOURCES} ${TEST_SOURCES})
add_library(q-xdg SHARED ${QXDG_SOURCES} ${QXDG_HEADERS} ${QXDG_PRIVATE_HEADERS})
set_target_properties(q-xdg PROPERTIES COMPILE_FLAGS "-DXDG_LIBRARY")
target_link_libraries(q-xdg ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

if( NOT XDG_NOT_BUILD_TEST )
    set(TEST_SOURCES main.cpp)
    add_executable(qxdgtest ${TEST_SOURCES})
    target_link_libraries(qxdgtest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} q-xdg)
    set_target_properties(qxdgtest PROPERTIES COMPILE_FLAGS "-DQT_GUI_LIB")
    add_dependencies(qxdgtest q-xdg)
endif( NOT XDG_NOT_BUILD_TEST )

set_target_properties(q-xdg PROPERTIES VERSION "0.1" SOVERSION "0")
install(TARGETS q-xdg DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${QXDG_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/q-xdg)
