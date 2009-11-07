# Copyright (c) 2009, Konstantin Plotnikov <kostyapl@gmail.com>
# Redistribution and use is allowed according to the terms of the GPL v3.

macro ( FIND_QUTIM_03 src_dir )
    if ( NOT FOUND_QUTIM_03 )
        find_path( QUTIM_INCLUDE_DIRS NAMES "libqutim/plugin.h" PATHS "../../" "${src_dir}/../../" )
        if ( QUTIM_INCLUDE_DIRS )
            message ( "Found qutim at \"${QUTIM_INCLUDE_DIRS}\"" )
            # This is very very nasty hack:
            set ( QUTIM_INCLUDE_DIRS ${QUTIM_INCLUDE_DIRS} "${QUTIM_INCLUDE_DIRS}/src"  )
            set ( FOUND_QUTIM_03 TRUE )
        else ( QUTIM_INCLUDE_DIRS )
            message ( FATAL_ERROR "Could not find qutIM development headers" )
        endif ( QUTIM_INCLUDE_DIRS )
    endif ( NOT FOUND_QUTIM_03 )
endmacro ( FIND_QUTIM_03 )

macro ( PREPARE_QUTIM_PLUGIN src_dir )
    if ( NOT QUTIM_PLUGIN )
        if ( NOT QT4_INSTALLED )
            # Require QT 4.6
            SET( QT_MIN_VERSION "4.6.0" )

            # Set QT modules
            SET( QT_USE_QTNETWORK TRUE )
            SET( QT_USE_QTGUI FALSE )

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

macro ( ADD_QUTIMPLUGIN_EXT2 plugin_name src_dir hdr_dir link_libraries )
    PREPARE_QUTIM_PLUGIN(${src_dir})
    if(MINGW)
        add_definitions( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
        list( APPEND COMPILE_FLAGS "-mthreads" )
    endif(MINGW)

    file( GLOB ${plugin_name}_SRC "${src_dir}/*.cpp" )
    file( GLOB ${plugin_name}_HDR "${src_dir}/*.h" )
    file( GLOB ${plugin_name}_UI "${src_dir}/*.ui" )

    INCLUDE_DIRECTORIES ( ${hdr_dir} )
    INCLUDE_DIRECTORIES ( ${src_dir} )
    INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR} )
    INCLUDE_DIRECTORIES ( ${QUTIM_INCLUDE_DIRS} )

    QT4_WRAP_CPP (${plugin_name}_MOC_SRC ${${plugin_name}_HDR})
    QT4_WRAP_UI (${plugin_name}_UI_H ${${plugin_name}_UI})
    #QT4_ADD_RESOURCES (${plugin_name}_RCC ${${plugin_name}_RES})

    # This project will generate library
    ADD_LIBRARY( ${plugin_name} SHARED ${${plugin_name}_SRC} ${${plugin_name}_MOC_SRC} ${${plugin_name}_UI_H} )

    # Link with QT
    TARGET_LINK_LIBRARIES( ${plugin_name} ${QT_LIBRARIES} ${link_libraries})

endmacro ( ADD_QUTIMPLUGIN_EXT2 )

macro ( ADD_QUTIMPLUGIN_EXT plugin_name src_dir hdr_dir )
    ADD_QUTIMPLUGIN_EXT2(${plugin_name} ${src_dir} ${hdr_dir} "" )
endmacro ( ADD_QUTIMPLUGIN_EXT )

macro ( ADD_QUTIMPLUGIN plugin_name src_dir )
    ADD_QUTIMPLUGIN_EXT2(${plugin_name} ${src_dir} "" "" )
endmacro ( ADD_QUTIMPLUGIN )

