#qutIM artwork install helper

#small workaround
if(NOT DEFINED IS_CPACK_INCLUDED)
	include(CPack)
	set(IS_CPACK_INCLUDED "true")
endif()


macro(QUTIM_ADD_ARTWORK_DIR art_dir)
	FILE(GLOB ARTWORKS ${art_dir} "${art_dir}/*")
	FOREACH(ART ${ARTWORKS})
 		IF(IS_DIRECTORY ${ART} AND NOT "${CMAKE_CURRENT_SOURCE_DIR}/${art_dir}" STREQUAL ${ART})
			QUTIM_ADD_ARTWORK(${ART})
 		ENDIF()
	ENDFOREACH()
endmacro()

IF(WIN32)
	SET(QUTIM_SHARE_DIR_DEF share)
ELSEIF(APPLE)
	SET(QUTIM_SHARE_DIR_DEF Resources/share)
ELSE()
	SET(QUTIM_SHARE_DIR_DEF share/apps/qutim)
ENDIF()
SET(QUTIM_SHARE_DIR "${QUTIM_SHARE_DIR_DEF}" CACHE DESCRIPTION "qutIM share dir")

macro(QUTIM_ADD_ARTWORK art)
	MESSAGE(STATUS "Added artwork: ${art} to ${QUTIM_SHARE_DIR}")
	INSTALL(DIRECTORY ${art}
		DESTINATION ${QUTIM_SHARE_DIR}
		COMPONENT Artwork
	)
endmacro()

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

MACRO (QUTIM_ADD_OPTION opt desc text def)
	OPTION (${opt} ${desc} ${def})
	IF (${opt})
		MESSAGE(STATUS ${text})
		ADD_DEFINITIONS("-D${opt}")
	ENDIF()
ENDMACRO()

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

		set ( QUTIM_PLUGIN TRUE )
		set ( QUTIM_PLUGIN_NULL "" )
	endif ( NOT QUTIM_PLUGIN )
endmacro ( __PREPARE_QUTIM_PLUGIN )

# Parameters:
#   plugin_name - name of plugin being added
macro (QUTIM_ADD_PLUGIN plugin_name)
	qutim_parse_arguments(QUTIM_${plugin_name}
	"DISPLAY_NAME;DESCRIPTION;LINK_LIBRARIES;SOURCE_DIR;GROUP;DEPENDS;EXTENSION_HEADER;EXTENSION_CLASS;INCLUDE_DIRS;COMPILE_FLAGS"
	"SUBPLUGIN;EXTENSION;STATIC"
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

	cpack_add_component( "${plugin_name}Devel"
		DISPLAY_NAME ${QUTIM_${plugin_name}_DISPLAY_NAME}
		DESCRIPTION "${plugin_name} development files"}
		DEPENDS ${QUTIM_${plugin_name}_DEPENDS}
		GROUP "${QUTIM_${plugin_name}_GROUP}Devel"
	)

	if( "${QUTIM_BASE_LIBRARY_TYPE}" STREQUAL "STATIC" )
	set( QUTIM_${plugin_name}_STATIC TRUE )
	endif( "${QUTIM_BASE_LIBRARY_TYPE}" STREQUAL "STATIC" )

	if( QUTIM_${plugin_name}_STATIC )
	set( QUTIM_${plugin_name}_LIBRARY_TYPE STATIC )
	else( QUTIM_${plugin_name}_STATIC )
	set( QUTIM_${plugin_name}_LIBRARY_TYPE SHARED )
	endif( QUTIM_${plugin_name}_STATIC )

	if( NOT QUTIM_${plugin_name}_SOURCE_DIR )
	set( QUTIM_${plugin_name}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )
	endif( NOT QUTIM_${plugin_name}_SOURCE_DIR )

	file( GLOB_RECURSE QUTIM_${plugin_name}_SRC "${QUTIM_${plugin_name}_SOURCE_DIR}/*.cpp" )
	file( GLOB_RECURSE QUTIM_${plugin_name}_SRC_MM "${QUTIM_${plugin_name}_SOURCE_DIR}/*.mm" )
	file( GLOB_RECURSE QUTIM_${plugin_name}_HDR "${QUTIM_${plugin_name}_SOURCE_DIR}/*.h" )
	file( GLOB_RECURSE QUTIM_${plugin_name}_UI "${QUTIM_${plugin_name}_SOURCE_DIR}/*.ui" )
	file( GLOB_RECURSE QUTIM_${plugin_name}_RES "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc" )
	list( APPEND QUTIM_${plugin_name}_SRC ${QUTIM_${plugin_name}_SRC_MM} ${QUTIM_${plugin_name}_DEFAULT_ARGS})

	if( QUTIM_${plugin_name}_EXTENSION )
		# Generate ${plugin_name}plugin.cpp file
	if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp" )
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

#include \"${plugin_name}plugin.moc\
"
QUTIM_EXPORT_PLUGIN(${plugin_name}Plugin)
" )
	endif( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp" )
	QT4_GENERATE_MOC( "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.moc" )
	list( APPEND QUTIM_${plugin_name}_SRC
		"${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.cpp"
		"${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}plugin.moc"
	)
	endif( QUTIM_${plugin_name}_EXTENSION )

	__PREPARE_QUTIM_PLUGIN(${QUTIM_${plugin_name}_SOURCE_DIR})
	if( MINGW )
		add_definitions( -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT )
		list( APPEND COMPILE_FLAGS "-mthreads" )
	endif( MINGW )

	# message( "${QUTIM_${plugin_name}_SRC}" )

	include_directories(
		${QUTIM_INCLUDE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QUTIM_${plugin_name}_INCLUDE_DIRS}
		${QUTIM_${plugin_name}_SOURCE_DIR}
	)

	QT4_WRAP_CPP( QUTIM_${plugin_name}_MOC_SRC ${QUTIM_${plugin_name}_HDR} )
	QT4_WRAP_UI( QUTIM_${plugin_name}_UI_H ${QUTIM_${plugin_name}_UI} )
	QT4_ADD_RESOURCES( QUTIM_${plugin_name}_RCC ${QUTIM_${plugin_name}_RES} )

	# This project will generate library
	add_library( ${plugin_name} ${QUTIM_${plugin_name}_LIBRARY_TYPE} ${QUTIM_${plugin_name}_SRC} ${QUTIM_${plugin_name}_MOC_SRC}
		 ${QUTIM_${plugin_name}_HDR} ${QUTIM_${plugin_name}_UI_H} ${QUTIM_${plugin_name}_RCC} )
#    set_target_properties( ${plugin_name} PROPERTIES COMPILE_FLAGS "-D${plugin_name}_MAKE" )
	if( QUTIM_${plugin_name}_STATIC )
	if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}helper.cpp" )
		file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}helper.cpp"
"#include <QtCore/QtPlugin>

Q_IMPORT_PLUGIN(${plugin_name})
" )
	endif( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}helper.cpp" )
	set( QUTIM_${plugin_name}_COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -DQUTIM_STATIC_PLUGIN -DQUTIM_PLUGIN_INSTANCE=\"qt_plugin_instance_${plugin_name}\"" )
	list( APPEND QUTIM_ADDITIONAL_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${plugin_name}helper.cpp" )
	list( APPEND QUTIM_ADDITIONAL_LIBRARIES ${plugin_name} )
	endif( QUTIM_${plugin_name}_STATIC )
	set( QUTIM_ADDITIONAL_SOURCES "${QUTIM_ADDITIONAL_SOURCES}" CACHE INTERNAL "" )
	set( QUTIM_ADDITIONAL_LIBRARIES "${QUTIM_ADDITIONAL_LIBRARIES}" CACHE INTERNAL "" )
	if( CMAKE_COMPILER_IS_GNUCXX )
		set_target_properties( ${plugin_name} PROPERTIES COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -Wall -Werror" )
	else( CMAKE_COMPILER_IS_GNUCXX )
		set_target_properties( ${plugin_name} PROPERTIES COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS}" )
	endif( CMAKE_COMPILER_IS_GNUCXX )


	#if( QUTIM_${plugin_name}_STATIC STREQUAL "SHARED" ) #what the fucking going on?
		install( TARGETS ${plugin_name}
			RUNTIME DESTINATION bin/plugins
				COMPONENT ${plugin_name}
			LIBRARY DESTINATION lib/qutim/plugins
				COMPONENT ${plugin_name}
			ARCHIVE DESTINATION lib
				COMPONENT "${plugin_name}Devel"
		)
	#endif( QUTIM_${plugin_name}_STATIC STREQUAL "SHARED" )

	if ( QUTIM_COPY_PLUGINS_TO_BINARY_DIR AND QUTIM_BINARY_DIR AND NOT QUTIM_${plugin_name}_STATIC )
		get_target_property ( ${plugin_name}_LOCATION ${plugin_name} LOCATION )
		add_custom_command(
			TARGET ${plugin_name}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${${plugin_name}_LOCATION}
				${QUTIM_BINARY_DIR}/plugins/${CMAKE_SHARED_LIBRARY_PREFIX}${plugin_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
		)
	endif ( QUTIM_COPY_PLUGINS_TO_BINARY_DIR AND QUTIM_BINARY_DIR AND NOT QUTIM_${plugin_name}_STATIC )

	# Link with QT
	target_link_libraries( ${plugin_name} ${QT_LIBRARIES} ${QUTIM_LIBRARIES} ${QUTIM_${plugin_name}_LINK_LIBRARIES} )

	if( LANGUAGE AND NOT QUTIM_${plugin_name}_SUBPLUGIN AND NOT QUTIM_${plugin_name}_EXTENSION )
	language_update( ${plugin_name} ${LANGUAGE} "${CMAKE_CURRENT_SOURCE_DIR}" )
	endif( LANGUAGE AND NOT QUTIM_${plugin_name}_SUBPLUGIN AND NOT QUTIM_${plugin_name}_EXTENSION )
endmacro (QUTIM_ADD_PLUGIN)

macro (QUTIM_ADD_LIBRARY library_name)
	if( WIN32 )
	qutim_parse_arguments(QUTIM_${library_name}
		"DISPLAY_NAME;DESCRIPTION;GROUP"
		""
		${ARGN}
	)
	endif( WIN32 )
endmacro (QUTIM_ADD_LIBRARY)
