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
#include <qutim/iconmanagerinterface.h>
#include <knotification.h>
#include <kapplication.h>
#include <QDebug>
#include "notifyhelper.h"
#include <kdeversion.h>
#include <shared/shareddata.h>


KDENotificationLayer::KDENotificationLayer()
{
	cutCount = 200;
}

KDENotificationLayer::~KDENotificationLayer()
{
}

bool KDENotificationLayer::init(PluginSystemInterface*)
{
	m_component_data = KdeIntegration::activeComponent();
	return true;
}

QList<SettingsStructure> KDENotificationLayer::getLayerSettingsList()
{
	SettingsStructure settings;
	settings.settings_item = new QTreeWidgetItem();
	settings.settings_item->setText(0, QObject::tr("Notifications"));
	settings.settings_item->setIcon(0, Icon("events"));
	m_settings_helper = new KDENotificationSettingsHelper();
	m_settings_widget = new KNotifyConfigWidget(m_settings_helper);
	m_settings_helper->setWidget(m_settings_widget);
	m_settings_widget.data()->setApplication(m_component_data.componentName());
	settings.settings_widget = m_settings_helper;
	m_settings.append(settings);
	return m_settings;
}

void KDENotificationLayer::saveLayerSettings()
{
	if(m_settings_widget.isNull())
		return;
	m_settings_widget.data()->save();
}

void KDENotificationLayer::removeLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		delete settings.settings_item;
		delete settings.settings_widget;
	}
	m_settings_helper.data()->deleteLater();
	m_settings.clear();
}

void KDENotificationLayer::showPopup(const TreeModelItem &item, const QString &message, NotificationType type)
{
	notify(item, message, type);
}

void KDENotificationLayer::playSound(const TreeModelItem &item, NotificationType type)
{
	notify(item, QString(), type);
}

void KDENotificationLayer::notify(const TreeModelItem &item, const QString &message, NotificationType type)
{
	QString title = "test title";
	//read contact and account info
	PluginSystemInterface *ps = SystemsCity::PluginSystem();
	QStringList contact_info;
	QStringList account_info;
	TreeModelItem account_item = item;
	account_item.m_item_name = item.m_account_name;
	QString contact_nick = item.m_item_name;
	QString contact_avatar;
	QString account_nick = item.m_account_name;
	QString account_avatar;
	QString n_type;
	if( type != NotifySystem )
	{
		contact_info = ps->getAdditionalInfoAboutContact(item);
		account_info = ps->getAdditionalInfoAboutContact(account_item);
		if ( contact_info.count() > 0)
		{
			contact_nick = contact_info.at(0);
		}
		if ( contact_info.count() > 1 )
		{
			contact_avatar = contact_info.at(1);
		}

		if ( account_info.count() > 0)
		{
			account_nick = account_info.at(0);
		}
		if ( account_info.count() > 1 )
		{
			account_avatar = account_info.at(1);
		}
	}
	switch ( type )
	{
	case NotifySystem:
		title = QObject::tr("System message from %1:").arg(item.m_account_name);
		n_type = "systemMessage";
		break;
	case NotifyStatusChange:
		title = contact_nick + "<br />";
		n_type = "NotifyStatusChange";
		break;
	case NotifyMessageGet:
		title = QObject::tr("Message from %1:").arg(contact_nick);
		n_type = "NotifyMessageGet";
		break;
	case NotifyTyping:
		title = QObject::tr("%1 is typing").arg(contact_nick);
		n_type = "NotifyTyping";
		break;
	case NotifyBlockedMessage:
		title = QObject::tr("Blocked message from %1").arg(contact_nick);
		n_type = "NotifyBlockedMessage";
		break;
	case NotifyBirthday:
		title = QObject::tr("%1 has birthday today!!").arg(contact_nick);
		n_type = "NotifyBirthday";
		break;
	case NotifyCustom:
		title = QObject::tr("Custom message for %1").arg(item.m_item_name);
		n_type = "NotifyCustom";
		break;
	case NotifyOnline:
		title = contact_nick;
		n_type = "NotifyOnline";
		break;
	case NotifyOffline:
		title = contact_nick;
		n_type = "NotifyOffline";
		break;
	case NotifyStartup:
		n_type = "NotifyStartup";
		break;
	case NotifyCount:
		n_type = "NotifyCount";
		break;
	default:
		return;
	}

	bool displayNewNotification = true;

	ActiveNotifications::iterator notifyIt = m_activenotifications.find(account_nick);
	if (notifyIt != m_activenotifications.end() && type == NotifyMessageGet)
	{
		(*notifyIt)->incrementMessages();
		displayNewNotification = false;
	}
	if (displayNewNotification)
	{

		KNotification *notification= new KNotification (n_type,
														0l,
														KNotification::CloseOnTimeout );
// 		if (message.length()>cutCount)
// 		{
// 			message.truncate(cutCount);
// 			message.append("...");
// 		}
		if( type != NotifySystem )
		{
			notification->setActions(QStringList() << tr("Open chat") << tr("Close"));
			NotifyHelper *notify_helper = new NotifyHelper (item);
			notify_helper->registerNotification(notification);
			connect(notification,SIGNAL(action1Activated()),notify_helper,SLOT(startChatSlot()));
			connect(notification,SIGNAL(action2Activated()),notify_helper,SLOT(closeSlot()));
			connect(notification,SIGNAL(destroyed(QObject*)),notify_helper,SLOT(deleteLater()));
			if( !contact_avatar.isEmpty() )
			{
				notification->setPixmap( QPixmap(contact_avatar) );
			}
		}
		//FIXME doesn't work(((((
//  		if( type == NotifyMessageGet ) {
//  			new ActiveNotification(notification,account_nick,m_activenotifications,message);
//  		}
//  		else
			notification->setText("<qt>"+message+"</qt>");
		notification->setTitle(title);
		notification->addContext("contact", contact_nick);
		notification->setComponentData(m_component_data);
		notification->sendEvent();
	}
}

void KDENotificationLayer::setLayerInterface(LayerType type, LayerInterface *layer_interface)
{
	LayersCity::instance().setLayerInterface( type, layer_interface );
}
