qutim_add_plugin( migration02x03
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/migrationpagecreator.h
	EXTENSION_CLASS Core::MigrationPageCreator
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Mirgation 0.2 to 0.3"
	DESCRIPTION "Module for migration from 0.2 to 0.3"
)
