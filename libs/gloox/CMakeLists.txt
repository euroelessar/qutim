file( GLOB gloox_SRCS *.cpp )

add_library( gloox SHARED ${gloox_SRCS} )
target_link_libraries( gloox ${LIBS} )
set_target_properties( gloox PROPERTIES SOVERSION 9 )
include_directories( ${gloox_SOURCE_DIR} )
add_definitions( -DGLOOX_EXPORTS -DDLL_EXPORT )
