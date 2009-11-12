CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0011 NEW)
ENDIF (COMMAND cmake_policy)
PROJECT ( jabber )
SET ( QT_MIN_VERSION "4.6.0" )

SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/" )

INCLUDE ( "libs/CMakeLists.txt" )