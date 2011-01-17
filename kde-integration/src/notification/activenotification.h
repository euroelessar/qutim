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

#ifndef ACTIVENOTIFICATION_H
#define ACTIVENOTIFICATION_H

#include <QHash>
class KNotification;


class ActiveNotification;

typedef QHash<QString, ActiveNotification *> ActiveNotifications;

class ActiveNotification : public QObject
{
	Q_OBJECT
	
	public:
		/**
		* Construct an active notification and add to the specified hash.
		* notification becomes the parent object
		*/
		ActiveNotification( KNotification *notification, const QString& id_, ActiveNotifications& notifications_, const QString& body_ );
		
		/**
		* Remove active notification from queue
		*/
		~ActiveNotification();
		
		/**
		* received another message from a sender with a notification
		*/
		void incrementMessages();
		
	private:
		QString              id;
		ActiveNotifications& notifications;
		int                  nEventsSinceNotified;
		
		/**
		* This is the text of the body minus the <qt> and </qt> around it
		* and without the "n more messages" part
		*/
		QString              body;
};


#endif // ACTIVENOTIFICATION_H
