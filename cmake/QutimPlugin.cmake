# Copyright (c) 2009, Konstantin Plotnikov <kostyapl@gmail.com>
# Redistribution and use is allowed according to the terms of the GPL v3.

# TODO: add install and uninstall rules
# TODO: change usage of relative path to absolute (src_dir)


# This macro is for internal use only
macro ( LANGUAGE_UPDATE plugin_name language sources )
	file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/languages/${language}" )
	if( NOT LANGUAGE_DEST_DIR )
		set( LANGUAGE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/languages" )
	endif( NOT LANGUAGE_DEST_DIR )
	separate_arguments( LANGUAGE_OPTS )
	execute_process( COMMAND ${QT_LUPDATE_EXECUTABLE}
					 ${LANGUAGE_OPTS}
					 -extensions "h,cpp,mm,js,c,ui"
					 -target-language "${language}" ${sources}
					 -ts "${LANGUAGE_DEST_DIR}/${language}/${plugin_name}.ts"
					 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endmacro ( LANGUAGE_UPDATE plugin_name language sources )

# This macro is for internal use only
macro ( FIND_QUTIM_03 src_dir )
    if ( NOT FOUND_QUTIM_03 )
        find_path( QUTIM_INCLUDE_DIRS NAMES "qutim/plugin.h" PATHS "../../" "../" "${src_dir}/../../" "${src_dir}/../" )
	find_library( QUTIM_LIBRARY qutim PATHS ${QUTIM_LIBRARY_DIR} )
        if ( QUTIM_INCLUDE_DIRS AND QUTIM_LIBRARY )
            message ( "Found qutim: ${QUTIM_LIBRARY}" )
            # This is very very nasty hack:
            set ( QUTIM_INCLUDE_DIRS ${QUTIM_INCLUDE_DIRS} "${QUTIM_INCLUDE_DIRS}/src"  )
            set ( FOUND_QUTIM_03 TRUE )
        else ( QUTIM_INCLUDE_DIRS AND QUTIM_LIBRARY )
            message ( FATAL_ERROR "Could not find qutIM development headers" )
        endif ( QUTIM_INCLUDE_DIRS AND QUTIM_LIBRARY )
    endif ( NOT FOUND_QUTIM_03 )
endmacro ( FIND_QUTIM_03 )

# This macro is for internal use only
macro ( PREPARE_QUTIM_PLUGIN src_dir )
    if ( NOT QUTIM_PLUGIN )
        if ( NOT QT4_INSTALLED )
            # Require QT 4.6
            SET( QT_MIN_VERSION "4.6.0" )

            # Set QT modules
            # SET( QT_USE_QTNETWORK TRUE )
            SET( QT_USE_QTGUI TRUE )

            # Search for QT4
            FIND_PACKAGE( Qt4 REQUIRED )

            # Include QT4
            INCLUDE( ${QT_USE_FILE} )
        endif ( NOT QT4_INSTALLED )

        FIND_QUTIM_03( ${src_dir} )

        set ( QUTIM_PLUGIN TRUE )
        set ( QUTIM_PLUGIN_NULL "" )
    endif ( NOT QUTIM_PLUGIN )
endmacro ( PREPARE_QUTIM_PLUGIN )

# Parameters:
#   plugin_name - name of plugin being added
#   src_dir - folder with sources
#   hdr_dir - folder with additional headers
#   libs_to_link - additional libraries to link with
macro ( ADD_QUTIMPLUGIN_EXT2 plugin_name src_dir hdr_dir libs_to_link )

    # Prepare path variables
    # TODO: fix this to make reverse convertion and fix later usage
    if ( IS_ABSOLUTE ${src_dir} )
        file ( RELATIVE_PATH src_dir "${CMAKE_CURRENT_SOURCE_DIR}/" ${src_dir} )
    endif ( IS_ABSOLUTE ${src_dir} )

    PREPARE_QUTIM_PLUGIN(${src_dir})
    if(MINGW)
        add_definitions( -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT )
        list( APPEND COMPILE_FLAGS "-mthreads" )
    endif(MINGW)

    file( GLOB_RECURSE ${plugin_name}_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/*.cpp" )
    file( GLOB_RECURSE ${plugin_name}_HDR "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/*.h" )
    file( GLOB_RECURSE ${plugin_name}_UI "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}/*.ui" )
    file( GLOB_RECURSE ${plugin_name}_RES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" )	
    message( "${${plugin_name}_SRC}" )

    INCLUDE_DIRECTORIES ( ${hdr_dir} )
    INCLUDE_DIRECTORIES ( ${src_dir} )
    INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR} )
    INCLUDE_DIRECTORIES ( ${QUTIM_INCLUDE_DIRS} )

    QT4_WRAP_CPP (${plugin_name}_MOC_SRC ${${plugin_name}_HDR})
    QT4_WRAP_UI (${plugin_name}_UI_H ${${plugin_name}_UI})
    QT4_ADD_RESOURCES (${plugin_name}_RCC ${${plugin_name}_RES})

    # This project will generate library
	ADD_LIBRARY( ${plugin_name} SHARED ${${plugin_name}_SRC} ${${plugin_name}_MOC_SRC} ${${plugin_name}_HDR} ${${plugin_name}_UI_H} ${${plugin_name}_RCC} )
    if( CMAKE_COMPILER_IS_GNUCXX )
        set_target_properties( ${plugin_name} PROPERTIES COMPILE_FLAGS "-Wall -Werror" )
    endif( CMAKE_COMPILER_IS_GNUCXX )
    INSTALL( TARGETS ${plugin_name} DESTINATION "lib/qutim/plugins" )

    # Link with QT
	TARGET_LINK_LIBRARIES( ${plugin_name} ${QT_LIBRARIES} ${QUTIM_LIBRARY} ${libs_to_link} ${ARGN})

	if( LANGUAGE )
		LANGUAGE_UPDATE( ${plugin_name} ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
	endif( LANGUAGE )

endmacro ( ADD_QUTIMPLUGIN_EXT2 )

# Parameters:
#   plugin_name - name of plugin being added
#   src_dir - folder with sources
#   hdr_dir - folder with additional headers
macro ( ADD_QUTIMPLUGIN_EXT plugin_name src_dir hdr_dir )
    ADD_QUTIMPLUGIN_EXT2(${plugin_name} ${src_dir} ${hdr_dir} "" )
endmacro ( ADD_QUTIMPLUGIN_EXT )

# Parameters:
#   plugin_name - name of plugin being added
#   src_dir - folder with sources
macro ( ADD_QUTIMPLUGIN plugin_name src_dir )
    ADD_QUTIMPLUGIN_EXT2(${plugin_name} ${src_dir} "" "" )
endmacro ( ADD_QUTIMPLUGIN )


# Please, do not use this macro it is not ready yet
# it is here because I need to store it somewhere =)
# macro ( ADD_QUTIMPLUGIN_AUTO plugin_name )
# 
#     # search for sources
#     if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src" )
#         set ( ${plugin_name}_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )
#     else ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src" )
#         set ( ${plugin_name}_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
#     endif ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src" )
# 
#     # search for resources
#     file( GLOB_RECURSE ${plugin_name}_QRC "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" )
#     if ( NOT ${plugin_name}_QRC )
#         if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons" )
#             set ( ${plugin_name}_RESOURCES TRUE )
#             set ( ${plugin_name}_ICONS "" )
#             set ( extensions
#                     png
#                     jpg
#                     gif
#                 )
#             foreach ( extension ${extensions} )
#                 file ( GLOB_RECURSE ICONS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/icons/*.${extension}" )
#                 list ( APPEND ${plugin_name}_ICONS ${ICONS} )
#             endforeach ( extension )
#             file ( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}.qrc" "<RCC>\n  <qresource prefix=\"${CMAKE_CURRENT_SOURCE_DIR}\">\n" )
#             foreach ( ICON ${${plugin_name}_ICONS} )
#                 file ( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}.qrc" "    <file>${ICON}</file>\n" )
#             endforeach ( ICON )
#         else ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons" )
#             set ( ${plugin_name}_RESOURCES FALSE )
#         endif ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons" )
#     endif ( NOT ${plugin_name}_QRC )
# endmacro ( QUTIMPLUGIN plugin_name )
