# Integration plugins


find_package( KDE4 "4.3")

if( KDE4_FOUND AND NOT DEFINED "NO_KDE" )
	message( "+ plugin kde added to build" )
	add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/kde" "${CMAKE_CURRENT_BINARY_DIR}/kde" )
else( KDE4_FOUND AND NOT DEFINED "NO_KDE" )
	message( "- plugin kde will be skipped" )
endif( KDE4_FOUND AND NOT DEFINED "NO_KDE" )
