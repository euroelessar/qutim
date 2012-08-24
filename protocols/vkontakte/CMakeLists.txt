set(QT_USE_QTNETWORK true)

option(SYSTEM_VREEN "Force use system vreen instead build-in copy" OFF)

if(SYSTEM_VREEN)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
	pkg_check_modules(VREEN REQUIRED vreen)
	pkg_check_modules(VREENOAUTH REQUIRED vreenoauth)
    endif()
endif()
if(NOT VREEN_FOUND)
    if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
	set(VREEN_IMPORTS_DIR bin)
    endif()
    set(VREEN_WITH_QMLAPI OFF CACHE INTERNAL "")
    set(VREEN_WITH_OAUTH ON CACHE INTERNAL "")
    set(VREEN_INSTALL_HEADERS OFF CACHE INTERNAL "")
    add_subdirectory(vreen)
    message(STATUS "Using internal copy of vreen")
    set(VREEN_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/vreen/include CACHE INTERNAL "")
    set(VREENOAUTH_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/vreen/include CACHE INTERNAL "")
    set(VREEN_LIBRARIES vreen CACHE INTERNAL "")
    set(VREENOAUTH_LIBRARIES vreenoauth CACHE INTERNAL "")
endif()

qutim_add_plugin(vkontakte
    DISPLAY_NAME "vkontakte"
    DESCRIPTION "New implementation of vk.com protocol"
    GROUP "Protocols"
    INCLUDE_DIRS ${VREEN_INCLUDE_DIRS} ${VREENOAUTH_INCLUDE_DIRS}
    LINK_LIBRARIES ${VREEN_LIBRARIES} ${VREENOAUTH_LIBRARIES}
    QT_LIBRARIES webkit declarative
)

qutim_add_artwork(share/vphotoalbum)
add_custom_directory(share/vphotoalbum DESCRIPTION "VPhotoalbum")
