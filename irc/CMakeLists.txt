CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( Irc )

SET( QT_USE_QTNETWORK TRUE )
qutim_add_plugin( irc
	DISPLAY_NAME "IRC protocol"
	DESCRIPTION "Implementation of IRC protocol"
	GROUP "Protocols"
)