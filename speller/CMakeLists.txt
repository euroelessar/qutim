# KDE Speller

file( GLOB_RECURSE speller_H src/*.h )
file( GLOB_RECURSE speller_CPP src/*.cpp )
file( GLOB_RECURSE speller_UI src/*.ui )

set( speller_SRCS
   ${speller_H}
   ${speller_CPP}
   kdespeller.qrc
)

QT4_WRAP_CPP( speller_SRCS ${speller_H} )
QT4_WRAP_UI( speller_SRCS ${speller_UI} )
QT4_ADD_RESOURCES( speller_SRCS ${speller_UI} )

add_library( kdespeller SHARED ${speller_SRCS} )

target_link_libraries( kdespeller
   ${QT_LIBRARIES}
   ${KDE4_KUTILS_LIBS}
)

install( TARGETS kdespeller DESTINATION lib/qutim )