# - Try to find qutIM development headers
# Once done this will define
#
#  QUTIM_FOUND - system has qutIM headers
#  QUTIM_INCLUDE_DIR - the qutIM include directory
#
# Copyright (c) 2009, Konstantin Plotnikov <kostyapl@gmail.com>
# Copyright (c) 2009-2010, Ruslan Nigmatullin, <euroelessar@gmail.com>

include( CPack )

# Argument-parsing macro from http://www.cmake.org/Wiki/CMakeMacroParseArguments
MACRO(QUTIM_PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})            
    SET(larg_names ${arg_names})    
    LIST(FIND larg_names "${arg}" is_arg_name)                   
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})    
      LIST(FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
	     SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
	     SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(QUTIM_PARSE_ARGUMENTS)


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
macro ( __FIND_QUTIM_03 src_dir )
    if ( NOT FOUND_QUTIM_03 )
	if( QUTIM_PATH )
	    SET(QUTIM_INCLUDE_DIRS ${QUTIM_PATH}/libqutim/include)
	    SET(QUTIM_LIBRARY_DIR ${QUTIM_PATH}/libqutim/)
	endif( QUTIM_PATH )

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
endmacro ( __FIND_QUTIM_03 )

# This macro is for internal use only
macro ( __PREPARE_QUTIM_PLUGIN src_dir )
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

        __FIND_QUTIM_03( ${src_dir} )

        set ( QUTIM_PLUGIN TRUE )
        set ( QUTIM_PLUGIN_NULL "" )
    endif ( NOT QUTIM_PLUGIN )
endmacro ( __PREPARE_QUTIM_PLUGIN )

# Parameters:
#   plugin_name - name of plugin being added
macro (QUTIM_ADD_PLUGIN plugin_name)
    qutim_parse_arguments(QUTIM_${plugin_name}
	"DISPLAY_NAME;DESCRIPTION;LINK_LIBRARIES;SOURCE_DIR;GROUP;DEPENDS;EXTENSION_HEADER;EXTENSION_CLASS"
	"SUBPLUGIN;EXTENSION"
	${ARGN}
    )
    if( NOT QUTIM_${plugin_name}_GROUP )
	set( QUTIM_${plugin_name}_GROUP Plugin )
    endif( NOT QUTIM_${plugin_name}_GROUP )
    cpack_add_component( ${plugin_name}
	DISPLAY_NAME ${QUTIM_${plugin_name}_DISPLAY_NAME}
	DESCRIPTION ${QUTIM_${plugin_name}_DESCRIPTION}
	DEPENDS ${QUTIM_${plugin_name}_DEPENDS}
	GROUP ${QUTIM_${plugin_name}_GROUP}
    )

    if( NOT QUTIM_${plugin_name}_SORUCE_DIR )
	set( QUTIM_${plugin_name}_SORUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )
    endif( NOT QUTIM_${plugin_name}_SORUCE_DIR )
    file( GLOB_RECURSE QUTIM_${plugin_name}_SRC "${QUTIM_${plugin_name}_SORUCE_DIR}/*.cpp" )
    file( GLOB_RECURSE QUTIM_${plugin_name}_SRC_MM "${QUTIM_${plugin_name}_SORUCE_DIR}/*.mm" )
    file( GLOB_RECURSE QUTIM_${plugin_name}_HDR "${QUTIM_${plugin_name}_SORUCE_DIR}/*.h" )
    file( GLOB_RECURSE QUTIM_${plugin_name}_UI "${QUTIM_${plugin_name}_SORUCE_DIR}/*.ui" )
    file( GLOB_RECURSE QUTIM_${plugin_name}_RES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" )
    list( APPEND QUTIM_${plugin_name}_SRC ${QUTIM_${plugin_name}_SRC_MM} ${QUTIM_${plugin_name}_DEFAULT_ARGS})
    
    if( QUTIM_${plugin_name}_EXTENSION )
        # Generate ${plugin_name}plugin.cpp file
        file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp"
"#include \"${QUTIM_${plugin_name}_EXTENSION_HEADER}\"
#include <qutim/plugin.h>

class ${plugin_name}Plugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
	virtual void init()
	{
		qutim_sdk_0_3::ExtensionIcon icon(QLatin1String(\"${QUTIM_${plugin_name}_ICON}\"));
		qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP(\"Plugin\", \"${QUTIM_${plugin_name}_DISPLAY_NAME}\");
		qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP(\"Plugin\", \"${QUTIM_${plugin_name}_DESCRIPTION}\");
		setInfo(name, description, QUTIM_VERSION, icon);
		addExtension<${QUTIM_${plugin_name}_EXTENSION_CLASS}>(name, description, icon);
	}
	virtual bool load() { return true; }
	virtual bool unload() { return true; }
};

QUTIM_EXPORT_PLUGIN(${plugin_name}Plugin)
" )
	list( APPEND QUTIM_${plugin_name}_SRC "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp")
    endif( QUTIM_${plugin_name}_EXTENSION )

    __PREPARE_QUTIM_PLUGIN("${CMAKE_CURRENT_SOURCE_DIR}/src")
    if( MINGW )
        add_definitions( -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT )
        list( APPEND COMPILE_FLAGS "-mthreads" )
    endif( MINGW )

    message( "${QUTIM_${plugin_name}_SRC}" )

    include_directories( ${QUTIM_INCLUDE_DIRS} )

    QT4_WRAP_CPP (${plugin_name}_MOC_SRC ${${plugin_name}_HDR})
    QT4_WRAP_UI (${plugin_name}_UI_H ${${plugin_name}_UI})
    QT4_ADD_RESOURCES (${plugin_name}_RCC ${${plugin_name}_RES})

    # This project will generate library
    add_library( ${plugin_name} SHARED ${QUTIM_${plugin_name}_SRC} ${QUTIM_${plugin_name}_MOC_SRC}
		 ${QUTIM_${plugin_name}_HDR} ${QUTIM_${plugin_name}_UI_H} ${QUTIM_${plugin_name}_RCC} )
    if( CMAKE_COMPILER_IS_GNUCXX )
        set_target_properties( ${plugin_name} PROPERTIES COMPILE_FLAGS "-Wall -Werror" )
    endif( CMAKE_COMPILER_IS_GNUCXX )
    install( TARGETS ${plugin_name} DESTINATION "lib/qutim/plugins" )

    # Link with QT
    target_link_libraries( ${plugin_name} ${QT_LIBRARIES} ${QUTIM_LIBRARY} ${QUTIM_${plugin_name}_LINK_LIBRARIES} )

    if( LANGUAGE AND NOT QUTIM_${plugin_name}_SUBPLUGIN AND NOT QUTIM_${plugin_name}_EXTENSION )
	language_update( ${plugin_name} ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
    endif( LANGUAGE AND NOT QUTIM_${plugin_name}_SUBPLUGIN AND NOT QUTIM_${plugin_name}_EXTENSION )
endmacro (QUTIM_ADD_PLUGIN)

macro (QUTIM_ADD_SUBPLUGIN plugin_name)
    set ( QUTIM_${plugin_name}_IS_SUBPLUGIN TRUE )
    qutim_add_plugin( ${plugin_name} ${ARGN} )
endmacro (QUTIM_ADD_SUBPLUGIN)

macro (QUTIM_ADD_EXTENSION plugin_name)
    STRING(TOUPPER ${plugin_name} plugin_uname)
#    CPACK_COMPONENT_${plugin_uname}_DISPLAY_NAME
#    CPACK_COMPONENT_${plugin_uname}_DESCRIPTION
    qutim_add_plugin( ${plugin_name} ${ARGN} ${QUTIM_${plugin_name}_EXT_SRC} )
endmacro (QUTIM_ADD_EXTENSION)
