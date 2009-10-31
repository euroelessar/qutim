# KDE Emoticons

file( GLOB_RECURSE emoticons_H src/*.h )
file( GLOB_RECURSE emoticons_CPP src/*.cpp )
file( GLOB_RECURSE emoticons_UI src/*.ui )

set( emoticons_SRCS
   ${emoticons_H}
   ${emoticons_CPP}
   kdeemoticons.qrc
)

QT4_WRAP_CPP( emoticons_SRCS ${emoticons_H} )
QT4_WRAP_UI( emoticons_SRCS ${emoticons_UI} )
QT4_ADD_RESOURCES( emoticons_SRCS ${emoticons_UI} )

add_library( kdeemoticons SHARED ${emoticons_SRCS} )

target_link_libraries( kdeemoticons
   ${QT_LIBRARIES}
   ${KDE4_KUTILS_LIBS}
)

install( TARGETS kdeemoticons DESTINATION lib/qutim )