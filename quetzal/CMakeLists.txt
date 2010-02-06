CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Quetzal )

include_directories( "/usr/include/libpurple" "/usr/include/glib-2.0" "/usr/lib/glib-2.0/include" )

SET( QT_USE_QTNETWORK TRUE )
ADD_QUTIMPLUGIN( "quetzal" "src" )
