project(q-xdg)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(QXDG_SOURCES
    src/xdgenvironmentmap.cpp
    src/xdgicontheme.cpp
    src/xdgiconmanager.cpp
    src/xdgthemechooser.cpp
)

set(QXDG_HEADERS
    src/xdg.h
    src/xdgenvironmentmap.h
    src/xdgicontheme.h
	src/xdgicontheme_p.h
    src/xdgiconmanager.h
    src/xdgthemechooser.h
)

set(TEST_SOURCES main.cpp)

qt4_automoc(${QXDG_SOURCES} ${TEST_SOURCES})
add_library(q-xdg SHARED ${QXDG_SOURCES} ${QXDG_HEADERS})
target_link_libraries(q-xdg ${QT_QTCORE_LIBRARY})

add_executable(qxdgtest ${TEST_SOURCES})
target_link_libraries(qxdgtest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} q-xdg)
set_target_properties(qxdgtest PROPERTIES COMPILE_FLAGS "-DQT_GUI_LIB")
add_dependencies(qxdgtest q-xdg)

set_target_properties(q-xdg PROPERTIES VERSION "0.1" SOVERSION "0")
install(TARGETS q-xdg DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES q-xdg DESTINATION ${CMAKE_INSTALL_PREFIX}/include/q-xdg)
