# KDE Phonon
message( "${DATA_INSTALL_DIR}" )
include( FindPhonon )

if( PHONON_FOUND )

file( GLOB_RECURSE phonon_H src/*.h )
file( GLOB_RECURSE phonon_CPP src/*.cpp )
file( GLOB_RECURSE phonon_UI src/*.ui )

set( phonon_SRCS
   ${phonon_H}
   ${phonon_CPP}
)

include_directories( ${PHONON_INCLUDES} )

QT4_WRAP_CPP( phonon_SRCS ${phonon_H} )
QT4_WRAP_UI( phonon_SRCS ${phonon_UI} )
QT4_ADD_RESOURCES( phonon_SRCS ${phonon_UI} )

add_library( kdephonon SHARED ${phonon_SRCS} )

target_link_libraries( kdephonon
   ${QT_LIBRARIES}
   ${KDE4_KUTILS_LIBS}
   ${PHONON_LIBS}
)

install( TARGETS kdephonon DESTINATION lib/qutim )

else( PHONON_FOUND )

endif( PHONON_FOUND )
