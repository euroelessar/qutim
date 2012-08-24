if(POLICY CMP0011)
	CMAKE_POLICY(SET CMP0011 NEW)
endif()
if(POLICY CMP0012)
	CMAKE_POLICY(SET CMP0012 NEW)
endif()
if(POLICY CMP0017)
	CMAKE_POLICY(SET CMP0017 NEW)
endif()

if(SYMBIAN)
	function(qutim_add_executable)
		symbian_add_executable(${ARGN})
	endfunction()
	function(qutim_target_link_libraries)
		symbian_target_link_libraries(${ARGN})
	endfunction()
else(SYMBIAN)
	function(qutim_add_executable)
		add_executable(${ARGN})
	endfunction()
	function(qutim_target_link_libraries)
		target_link_libraries(${ARGN})
	endfunction()
endif(SYMBIAN)

#include guards
if(NOT CPACK_INCLUDED)
	set(CPACK_INCLUDED TRUE)
	include(CPack)
endif()

#TODO separate to external macro
MACRO(QUTIM_WRAP_CPP outfiles)
	# get include dirs
	QT4_GET_MOC_FLAGS(moc_flags)
	QT4_EXTRACT_OPTIONS(moc_files moc_options ${ARGN})

	FOREACH (it ${moc_files})
		GET_FILENAME_COMPONENT(_abs_FILE ${it} ABSOLUTE)
		GET_FILENAME_COMPONENT(_abs_PATH ${_abs_FILE} PATH)
		GET_FILENAME_COMPONENT(_basename ${it} NAME_WE)

		SET(_HAS_MOC false)

		IF(EXISTS ${_abs_PATH}/${_basename}.cpp)
			SET(_header ${_abs_PATH}/${_basename}.cpp)
			FILE(READ ${_header} _contents)
			STRING(REGEX MATCHALL "# *include +[^ ]+\\.moc[\">]" _match "${_contents}")
			IF(_match)
				SET(_HAS_MOC true)
				FOREACH (_current_MOC_INC ${_match})
					STRING(REGEX MATCH "[^ <\"]+\\.moc" _current_MOC "${_current_MOC_INC}")
					SET(_moc    ${CMAKE_CURRENT_BINARY_DIR}/${_current_MOC})
					QT4_CREATE_MOC_COMMAND(${_abs_FILE} ${_moc} "${_moc_INCS}" "")
					MACRO_ADD_FILE_DEPENDENCIES(${_abs_FILE} ${_moc})
				ENDFOREACH (_current_MOC_INC)
			ENDIF()
		ENDIF()
		IF(NOT _HAS_MOC)
			FILE(READ ${_abs_FILE} _contents)
                        STRING(REGEX MATCHALL "Q_OBJECT|Q_GADGET" _match2 "${_contents}")
			IF(_match2)
				QT4_MAKE_OUTPUT_FILE(${_abs_FILE} moc_ cxx outfile)
				QT4_CREATE_MOC_COMMAND(${_abs_FILE} ${outfile} "${moc_flags}" "${moc_options}")
				SET(${outfiles} ${${outfiles}} ${outfile})
			ENDIF()
		ENDIF()
	ENDFOREACH(it)
ENDMACRO (QUTIM_WRAP_CPP)

macro(QUTIM_ADD_ARTWORK_DIR art_dir)
	FILE(GLOB ARTWORKS ${art_dir} "${art_dir}/*")
	GET_FILENAME_COMPONENT(_abs_PATH ${art_dir} ABSOLUTE)
	FOREACH(ART ${ARTWORKS})
		GET_FILENAME_COMPONENT(_abs_FILE ${ART} ABSOLUTE)
		IF(IS_DIRECTORY ${_abs_FILE} AND NOT ${_abs_PATH} STREQUAL ${_abs_FILE})
			QUTIM_ADD_ARTWORK(${_abs_FILE})
 		ENDIF()
	ENDFOREACH()
endmacro()

option(QUTIM_ENABLE_ALL_PLUGINS "Enable all extensions by default" ON)
option(QUTIM_DEVELOPER_BUILD "Install devel stuff" ON)

macro(qutim_add_devel_component compname)
	if(QUTIM_DEVELOPER_BUILD)
		cpack_add_component(compname)
	endif()
endmacro()

IF(WIN32)
	SET(QUTIM_SHARE_DIR_DEF share)
ELSEIF(APPLE)
	SET(QUTIM_SHARE_DIR_DEF ${QUTIM_BUNDLE_NAME}/Contents/Resources/share)
ELSE()
	SET(QUTIM_SHARE_DIR_DEF share/apps/qutim)
ENDIF()
#SET(QUTIM_SHARE_DIR "${QUTIM_SHARE_DIR_DEF}" CACHE DESCRIPTION "qutIM share dir")
set(QUTIM_SHARE_DIR "${QUTIM_SHARE_DIR_DEF}" CACHE STRING "QutIM share dir")

macro(qutim_add_artwork category_dir)
	file(GLOB artworks ${category_dir} "${category_dir}/*")
	get_filename_component(category ${category_dir} NAME_WE)
	get_filename_component(_abs_PATH ${category_dir} ABSOLUTE)
	FOREACH(art ${artworks})
		get_filename_component(_abs_FILE ${art} ABSOLUTE)
		if(IS_DIRECTORY ${_abs_FILE} AND NOT ${_abs_PATH} STREQUAL ${_abs_FILE})
			get_filename_component(_basename ${art} NAME_WE)
			string( TOUPPER ${category}/${_basename} ARTNAME)
			option(${ARTNAME} "" ${QUTIM_ENABLE_ALL_PLUGINS})
			if(NOT "${ARTNAME}")
				message(STATUS "[.] ${ARTNAME}: Directory ${_basename} will be skipped")
			else()
				message(STATUS "[*] ${ARTNAME}: Directory ${_basename} will be installed to ${QUTIM_SHARE_DIR}/${category}/${_basename}")
				install(DIRECTORY ${art}
					DESTINATION ${QUTIM_SHARE_DIR}/${category}
					COMPONENT Artwork #${category}
				)
			endif()
 		endif()
	endforeach()
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
#	file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/languages/${language}" )
#	if( NOT LANGUAGE_DEST_DIR )
#		set( LANGUAGE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/languages" )
#	endif( NOT LANGUAGE_DEST_DIR )
#	separate_arguments( LANGUAGE_OPTS )
#	execute_process( COMMAND ${QT_LUPDATE_EXECUTABLE}
#					 ${LANGUAGE_OPTS}
#					 -extensions "h,cpp,mm,js,c,ui"
#					 -target-language "${language}" ${sources}
#					 -ts "${LANGUAGE_DEST_DIR}/${language}/${plugin_name}.ts"
#					 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endmacro ( LANGUAGE_UPDATE plugin_name language sources )

# This macro is for internal use only
macro ( __PREPARE_QUTIM_PLUGIN src_dir )
	#CMAKE_POLICY(SET CMP0017 NEW)
	if ( NOT QUTIM_PLUGIN )
		if ( NOT QT4_INSTALLED )

			# Set QT modules
                        SET( QT_USE_QTNETWORK TRUE )
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
	"DISPLAY_NAME;ICON;DESCRIPTION;LINK_LIBRARIES;QT_LIBRARIES;SOURCE_DIR;DECLARATIVE_DIR;GROUP;DEPENDS;EXTENSION_HEADER;EXTENSION_CLASS;INCLUDE_DIRS;COMPILE_FLAGS"
	"SUBPLUGIN;EXTENSION;STATIC;"
	${ARGN}
	)
	if( NOT QUTIM_${plugin_name}_GROUP )
		set( QUTIM_${plugin_name}_GROUP Plugin )
	endif( NOT QUTIM_${plugin_name}_GROUP )

	# Link with Qt
	list( APPEND QUTIM_${plugin_name}_QT_LIBRARIES CORE GUI )
	foreach( USED_QT_MODULE_LOWER ${QUTIM_${plugin_name}_QT_LIBRARIES} )
		string( TOUPPER ${USED_QT_MODULE_LOWER} USED_QT_MODULE )
		if( NOT QT_QT${USED_QT_MODULE}_FOUND )
			message(STATUS "Qt${USED_QT_MODULE_LOWER} not found. Cannot build ${QUTIM_${plugin_name}_DISPLAY_NAME}")
			return()
		endif()
		list( APPEND QUTIM_${plugin_name}_LINK_LIBRARIES ${QT_QT${USED_QT_MODULE}_LIBRARY} )
		include_directories( ${QT_QT${USED_QT_MODULE}_INCLUDE_DIR} )
	endforeach()

	cpack_add_component( ${plugin_name}
		DISPLAY_NAME ${QUTIM_${plugin_name}_DISPLAY_NAME}
		DESCRIPTION ${QUTIM_${plugin_name}_DESCRIPTION}
		DEPENDS ${QUTIM_${plugin_name}_DEPENDS}
		GROUP ${QUTIM_${plugin_name}_GROUP}
	)

	qutim_add_devel_component( "${plugin_name}Devel"
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

	if( NOT QUTIM_${plugin_name}_DECLARATIVE_DIR )
		set( QUTIM_${plugin_name}_DECLARATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/declarative" )
	endif( NOT QUTIM_${plugin_name}_DECLARATIVE_DIR )

	file( GLOB_RECURSE QUTIM_${plugin_name}_SRC "${QUTIM_${plugin_name}_SOURCE_DIR}/*.cpp" )
	if(APPLE)
		file( GLOB_RECURSE QUTIM_${plugin_name}_SRC_MM "${QUTIM_${plugin_name}_SOURCE_DIR}/*.mm" )
	endif()
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

#include \"${plugin_name}plugin.moc\"
QUTIM_EXPORT_PLUGIN(${plugin_name}Plugin)
")
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
		${QUTIM_${plugin_name}_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QUTIM_INCLUDE_DIR}		
		${QUTIM_${plugin_name}_INCLUDE_DIRS}		
	)

	QUTIM_WRAP_CPP( QUTIM_${plugin_name}_MOC_SRC ${QUTIM_${plugin_name}_HDR} )
	QT4_WRAP_UI( QUTIM_${plugin_name}_UI_H ${QUTIM_${plugin_name}_UI} )

	#static resources should be added directly to qutim executable
	if( QUTIM_${plugin_name}_STATIC )
		list( APPEND QUTIM_ADDITIONAL_RCC "${QUTIM_${plugin_name}_RES}" )
		#QT4_ADD_RESOURCES( QUTIM_${plugin_name}_RCC_STATIC ${QUTIM_${plugin_name}_RES} )
	else()
		QT4_ADD_RESOURCES( QUTIM_${plugin_name}_RCC ${QUTIM_${plugin_name}_RES} )
	endif()

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
	set( QUTIM_ADDITIONAL_RCC "${QUTIM_ADDITIONAL_RCC}" CACHE INTERNAL "" )
	set( QUTIM_ADDITIONAL_LIBRARIES "${QUTIM_ADDITIONAL_LIBRARIES}" CACHE INTERNAL "" )

	#more effective compile flags
	if(MSVC)
		set(QUTIM_${plugin_name}_COMPILE_FLAGS
			"${QUTIM_${plugin_name}_COMPILE_FLAGS} /W3")
	else()
		if(SYMBIAN)
			#disable stupid warnings
			set(QUTIM_${plugin_name}_COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -w")
		else()
			set(QUTIM_${plugin_name}_COMPILE_FLAGS
				"${QUTIM_${plugin_name}_COMPILE_FLAGS} -Wall -Wextra -Wnon-virtual-dtor")
			if(NOT WIN32)
				set(QUTIM_${plugin_name}_COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -fvisibility=hidden")
				if(DEVELOPER)
					set(QUTIM_${plugin_name}_COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -Werror")
				endif(DEVELOPER)
			endif(NOT WIN32)
		endif()
	endif()
	if( NOT QUTIM_${plugin_name}_DEBUG_ID )
		string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef QUTIM_${plugin_name}_DEBUG_ID_TMP)
		set( QUTIM_${plugin_name}_DEBUG_ID ${QUTIM_${plugin_name}_DEBUG_ID_TMP} CACHE INTERNAL "" )
	endif( NOT QUTIM_${plugin_name}_DEBUG_ID )
	set(QUTIM_${plugin_name}_COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS} -DQUTIM_PLUGIN_ID=${QUTIM_${plugin_name}_DEBUG_ID}")
	set_target_properties(${plugin_name} PROPERTIES COMPILE_FLAGS "${QUTIM_${plugin_name}_COMPILE_FLAGS}")

	qutim_target_link_libraries( ${plugin_name} ${QUTIM_LIBRARY} ${QUTIM_${plugin_name}_LINK_LIBRARIES} )

	if( EXISTS ${QUTIM_${plugin_name}_DECLARATIVE_DIR} AND IS_DIRECTORY ${QUTIM_${plugin_name}_DECLARATIVE_DIR} )
		install(DIRECTORY ${QUTIM_${plugin_name}_DECLARATIVE_DIR}/
			DESTINATION ${QUTIM_SHARE_DIR}/declarative
			COMPONENT ${plugin_name}
		)
	endif()

	#if( QUTIM_${plugin_name}_STATIC STREQUAL "SHARED" ) #what the fucking going on?
		install( TARGETS ${plugin_name}
			RUNTIME DESTINATION bin/plugins
				COMPONENT ${plugin_name}
			LIBRARY DESTINATION lib${LIB_SUFFIX}/qutim/plugins
				COMPONENT ${plugin_name}
			ARCHIVE DESTINATION lib${LIB_SUFFIX}
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
	IF (APPLE AND NOT QUTIM_${plugin_name}_STATIC AND QUTIM_BUNDLE_LOCATION)
		IF (NOT EXISTS "${QUTIM_BUNDLE_LOCATION}/Contents/Resources/qt.conf")
			file (WRITE "${QUTIM_BUNDLE_LOCATION}/Contents/Resources/qt.conf"
"[Paths]
Plugins = PlugIns"
)
		ENDIF (NOT EXISTS "${QUTIM_BUNDLE_LOCATION}/Contents/Resources/qt.conf")
		add_dependencies(qutim-bundle ${plugin_name})
		get_target_property ( ${plugin_name}_LOCATION ${plugin_name} LOCATION )
		add_custom_command(
			TARGET ${plugin_name}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND}
				ARGS -E copy ${${plugin_name}_LOCATION}
				${QUTIM_BUNDLE_LOCATION}/Contents/PlugIns/${CMAKE_SHARED_LIBRARY_PREFIX}${plugin_name}${CMAKE_SHARED_LIBRARY_SUFFIX}

			)
	ENDIF(APPLE AND NOT QUTIM_${plugin_name}_STATIC AND QUTIM_BUNDLE_LOCATION)

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

macro(qutim_generate_includes include_dir)
	foreach( header ${ARGN} )
		GET_FILENAME_COMPONENT(_basename ${header} NAME_WE)
		GET_FILENAME_COMPONENT(_abs_FILE ${header} ABSOLUTE)
		#MESSAGE(STATUS ${_abs_FILE})

		if( NOT EXISTS "${QUTIM_INCLUDE_DIR}/${include_dir}/${_basename}.h" )
			file( WRITE "${QUTIM_INCLUDE_DIR}/${include_dir}/${_basename}.h"
		"#include \"${_abs_FILE}\"
				"
			)
		endif( NOT EXISTS "${QUTIM_INCLUDE_DIR}/${include_dir}/${_basename}.h" )
	endforeach( header )
endmacro()

macro(qutim_add_extensions_dir dir)
file(GLOB qutim_core_extensions "${dir}" "${dir}/*" )
foreach( extension ${qutim_core_extensions} )
    if(IS_DIRECTORY "${extension}" AND EXISTS "${extension}/CMakeLists.txt" )
		GET_FILENAME_COMPONENT(_basename ${extension} NAME_WE)
		string( TOUPPER ${_basename} SUBDIR_NAME)
		option(${SUBDIR_NAME} "" ${QUTIM_ENABLE_ALL_PLUGINS})
		if(${SUBDIR_NAME})
			add_subdirectory("${extension}/" "${CMAKE_CURRENT_BINARY_DIR}/${_basename}")
			message(STATUS "[+] ${SUBDIR_NAME}: extension added to build")
		else()
			message(STATUS "[-] ${SUBDIR_NAME}: extension will be skipped")
		endif()
    endif()
endforeach( extension )
endmacro()
