cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
project(qutim)

set(QUTIM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/core" CACHE FILEPATH "Path to qutim sources")
set(QUTIM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/core/libqutim/include" CACHE FILEPATH "Path to qutim headers")
set(QUTIM_LIBRARY libqutim CACHE FILEPATH "Path to qutim library")

option(QUTIM_ENABLE_ALL_PLUGINS "Enable plugins by default" ON)

list(APPEND CMAKE_MODULE_PATH "${QUTIM_PATH}/cmake")
include(QutIMMacros)

set(QUTIM_ADDITIONAL_PATHS "protocols" "plugins" )
set(QUTIM_ADDITIONAL_PATHS "${QUTIM_ADDITIONAL_PATHS}" CACHE INTERNAL "" FORCE )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core/" "${CMAKE_CURRENT_BINARY_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/translations/" "${CMAKE_CURRENT_BINARY_DIR}/translations")

# ADD_CUSTOM_TARGET(uninstall
#   "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
