cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
if( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
endif(COMMAND cmake_policy )
project( Plugins )
set(QT_MIN_VERSION "4.6.0")

set(QUTIM_PATH "" CACHE FILEPATH "Path to qutim distro")
set(CMAKE_INSTALL_PREFIX ${QUTIM_PATH})
#set(QUTIM_LIBRARIES_DIR "${QUTIM_PATH}")

LIST (APPEND CMAKE_MODULE_PATH "cmake")
LIST (APPEND CMAKE_MODULE_PATH "${QUTIM_PATH}/share/cmake/Modules")
LIST (APPEND CMAKE_PREFIX_PATH "${QUTIM_PATH}")

INCLUDE (MacroEnsureVersion)
find_package(Qt4 COMPONENTS QtCore QtGui)
find_package(QutIM REQUIRED)

include_directories(include)
add_subdirectory(src)
