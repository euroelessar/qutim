# KDE Emoticons

file( GLOB_RECURSE crash_H src/*.h )
file( GLOB_RECURSE crash_CPP src/*.cpp )
file( GLOB_RECURSE crash_UI src/*.ui )

set( crash_SRCS
   ${crash_H}
   ${crash_CPP}
   kdecrash.qrc
)

QT4_WRAP_CPP( crash_SRCS ${crash_H} )
QT4_WRAP_UI( crash_SRCS ${crash_UI} )
QT4_ADD_RESOURCES( crash_SRCS ${crash_UI} )

add_library( kdecrash SHARED ${crash_SRCS} )

target_link_libraries( kdecrash
   ${QT_LIBRARIES}
   ${KDE4_KUTILS_LIBS}
)

install( TARGETS kdecrash DESTINATION lib/qutim )