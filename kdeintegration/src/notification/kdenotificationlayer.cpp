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

#include "kdenotificationlayer.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/chatunit.h>
#include <qutim/account.h>
#include <qutim/message.h>
#include <knotification.h>
#include <kapplication.h>
#include <QDebug>
#include "notifyhelper.h"
#include <QTextDocument>
#include <kdeversion.h>
#include <shared/shareddata.h>


KDENotificationLayer::KDENotificationLayer()
{
	cutCount = 200;
	SettingsItem *appearance = new GeneralSettingsItem<KDENotificationSettingsHelper>(
			Settings::Appearance,
			Icon("dialog-information"),
			QT_TRANSLATE_NOOP("Settings","Popups"));
	Settings::registerItem(appearance);
}

KDENotificationLayer::~KDENotificationLayer()
{
}

void KDENotificationLayer::show(Notifications::Type type, QObject *sender, const QString &body, const QVariant &data)
{
	QString text = Qt::escape(body);
	QString sender_id = sender ? sender->property("id").toString() : QString();
	QString sender_name = sender ? sender->property("name").toString() : QString();
	if(sender_name.isEmpty())
		sender_name = sender_id;
	QString title = Notifications::toString(type).arg(sender_name);


	if (data.canConvert<Message>() && (type & Notifications::MessageSend & Notifications::MessageGet)) {
		const Message &msg = data.value<Message>();
		title = Notifications::toString(type).arg(msg.chatUnit()->title());
	} else if (data.canConvert<QString>()) {
		title = data.toString();
	}

	//read contact and account info
	QString sender_avatar = sender ? sender->property("avatar").toString() : QString();
	QString account_nick;
	if (ChatUnit *unit = qobject_cast<ChatUnit*>(sender))
		account_nick = unit->account()->id();
	QString n_type;
	switch (type) {
	case Notifications::Online:
		n_type = "Online";
		break;
	case Notifications::Offline:
		n_type = "Offline";
		break;
	case Notifications::StatusChange:
		n_type = "StatusChange";
		break;
	case Notifications::Birthday:
		n_type = "Birthday";
		break;
	case Notifications::Startup:
		n_type = "Startup";
		break;
	case Notifications::MessageGet:
		n_type = "MessageGet";
		break;
	case Notifications::MessageSend:
		n_type = "MessageSend";
		break;
	case Notifications::System:
		n_type = "System";
		break;
	case Notifications::Typing:
		n_type = "Typing";
		break;
	case Notifications::BlockedMessage:
		n_type = "BlockedMessage";
		break;
	default:
		return;
	}

	bool displayNewNotification = true;

	if (type == Notifications::MessageGet) {
		ActiveNotifications::iterator notifyIt = m_activenotifications.find(account_nick);
		if (notifyIt != m_activenotifications.end()) {
			(*notifyIt)->incrementMessages();
			displayNewNotification = false;
		}
	}
	if (displayNewNotification) {
		KNotification *notification= new KNotification(n_type, 0l, KNotification::CloseOnTimeout);
		// 		if (text.length()>cutCount)
		// 		{
		// 			text.truncate(cutCount);
		// 			text.append("...");
		// 		}
		if (type != Notifications::System) {
			notification->setActions(QStringList() << tr("Open chat") << tr("Close"));
			NotifyHelper *notify_helper = new NotifyHelper(qobject_cast<ChatUnit*>(sender), data);
			notify_helper->registerNotification(notification);
			connect(notification,SIGNAL(action1Activated()),notify_helper,SLOT(startChatSlot()));
			connect(notification,SIGNAL(action2Activated()),notify_helper,SLOT(closeSlot()));
			connect(notification,SIGNAL(destroyed(QObject*)),notify_helper,SLOT(deleteLater()));
			if (!sender_avatar.isEmpty())
				notification->setPixmap(QPixmap(sender_avatar));
		}
		//FIXME doesn't work(((((
		//  		if( type == NotifyMessageGet ) {
		//  			new ActiveNotification(notification,account_nick,m_activenotifications,text);
		//  		}
		//  		else
		notification->setText("<qt>" + text + "</qt>");
		notification->setTitle(title);
		notification->addContext("contact", sender_name);
		notification->setComponentData(KGlobal::activeComponent());
		notification->sendEvent();
	}
}
