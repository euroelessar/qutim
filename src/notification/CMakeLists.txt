# KDE Notifications for version 4.2

macro_ensure_version( "4.2.0" "${GENERIC_LIB_VERSION}" KDE_HAS_4_2 )
if( NOT KDE_HAS_4_2 )
        message( SEND_ERROR "Minimum kdelibs version is 4.2.0, but ${GENERIC_LIB_VERSION} found" )
endif( NOT KDE_HAS_4_2 )

file( GLOB_RECURSE notification_H src/*.h )
file( GLOB_RECURSE notification_CPP src/*.cpp )
file( GLOB_RECURSE notification_UI src/*.ui )

set( notification_SRCS
   ${notification_H}
   ${notification_CPP}
   icons.qrc
)

QT4_WRAP_CPP( notification_SRCS ${notification_H} )
QT4_WRAP_UI( notification_SRCS ${notification_UI} )
QT4_ADD_RESOURCES( notification_SRCS ${notification_UI} )

add_library( kdenotification SHARED ${notification_SRCS} )

target_link_libraries( kdenotification
   ${QT_LIBRARIES}
   ${KDE4_KDEUI_LIBS}
   ${KDE4_KNOTIFYCONFIG_LIBS}
)

add_subdirectory(pics)

install( TARGETS kdenotification DESTINATION lib/qutim )
install( FILES qutim.notifyrc DESTINATION "${DATA_INSTALL_DIR}/qutim" )
