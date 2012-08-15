option(SYSTEM_QTDOCKTILE "Force use system qtdocktile instead build-in copy" OFF)

if(SYSTEM_QTDOCKTILE)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
	pkg_check_modules(QT_QTDOCKTILE REQUIRED qtdocktile)
    endif()
endif()
if(NOT QT_QTDOCKTILE_FOUND)
    if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
	set(QT_QTDOCKTILE_IMPORTS_DIR bin)
	set(QT_QTDOCKTILE_PLUGINS_DIR bin/)
    endif()
    set(QT_QTDOCKTILE_WITH_QMLAPI OFF)
    set(QT_QTDOCKTILE_INSTALL_HEADERS OFF)
    add_subdirectory(qtdocktile)
    message(STATUS "Using internal copy of qtdocktile")
    set(QT_QTDOCKTILE_LIBRARIES qtdocktile CACHE INTERNAL "")
    set(QT_QTDOCKTILE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/qtdocktile/include/QtDockTile CACHE INTERNAL "")
endif()

qutim_add_plugin("docktile"
	DISPLAY_NAME "DockTile"
	DESCRIPTION "Dock integration"
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src"
	LINK_LIBRARIES	${QT_QTDOCKTILE_LIBRARIES}
	INCLUDE_DIRS ${QT_QTDOCKTILE_INCLUDE_DIRS}
)

