/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "activenotification.h"
#include <knotification.h>
#include <klocale.h>

ActiveNotification::ActiveNotification( KNotification *notification, const QString& id_, ActiveNotifications& notifications_, const QString& body_ )
: QObject( notification ), id( id_ ),
notifications( notifications_ ), nEventsSinceNotified( 0 ), body( body_ )
{
	notifications.insert( id, this );
	static_cast<KNotification *>( parent())->setText("<qt>" + body + "</qt>" );
}

/**
* Remove active notification from queue
*/
ActiveNotification::~ActiveNotification() {
	notifications.remove( id );
}

/**
* received another message from a sender with a notification
*/
void ActiveNotification::incrementMessages() {
	KLocalizedString append = ki18np( "+ %1 more message", "+ %1 more messages");
	KNotification *aParent = static_cast<KNotification *>( parent() );
	aParent->setText( QString( "<qt>" ) + body + "<br/><small><font color=\"yellow\">" + append.subs( ++nEventsSinceNotified ).toString() + "</small></font></qt>" );
}