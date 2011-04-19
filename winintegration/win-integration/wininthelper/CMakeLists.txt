
file(GLOB_RECURSE wi_helper_headers "src/*.h")
file(GLOB_RECURSE wi_helper_sources "src/*.cpp")

add_executable(wininthelper WIN32 ${wi_helper_sources} ${wi_helper_headers})
target_link_libraries(wininthelper "ws2_32")

install( TARGETS wininthelper
	RUNTIME DESTINATION bin
		COMPONENT wininthelper
	LIBRARY DESTINATION lib${LIB_SUFFIX}
		COMPONENT wininthelper
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
		COMPONENT wininthelper
)
