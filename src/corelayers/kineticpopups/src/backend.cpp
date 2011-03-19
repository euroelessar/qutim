/****************************************************************************
*  backend.cpp
*
*  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
*
***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*****************************************************************************/

#include "backend.h"
#include "popup.h"
#include "manager.h"
#include <QVariant>
#include <QTime>
#include <QDebug>
#include <qutim/debug.h>
#include <qutim/message.h>
#include <qutim/chatunit.h>
#include  <QTextDocument>
#include "settings/popupappearance.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core
{
namespace KineticPopups
{

Backend::Backend ()
{
	GeneralSettingsItem<PopupAppearance> *appearance = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance, Icon("dialog-information"), QT_TRANSLATE_NOOP("Settings","Popups"));
	Settings::registerItem(appearance);
	Q_UNUSED(QT_TRANSLATE_NOOP("Service","PopupWidget"));
}

void Backend::show(Notifications::Type type, QObject* sender, const QString& body,
				   const QVariant& data)
{
	Manager *manager =  Manager::self();
	if (!(manager->showFlags & type) || (manager->count() >= manager->maxCount)) {
		return;
	}

	static int id_counter = 0;
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

	QString popup_id = title;

	bool updateMode = manager->updateMode;
	bool appendMode = manager->appendMode;

	Popup *popup = manager->getById(popup_id);
	if (popup) {
		if (appendMode) {
			updateMode ? popup->updateMessage(text) : popup->appendMessage(text);
			return;
		} else if (sender) {
			popup_id.append("." + QString::number(id_counter++));
		}
	}
	popup  = new Popup (popup_id);
	popup->setMessage(title,text,sender);
	popup->setData(data);
	if (sender)
		connect(sender,SIGNAL(destroyed(QObject*)),popup,SLOT(deleteLater()));
	popup->send();
}

void Game::Backend::handleNotification(qutim_sdk_0_3::Notification* notification)
{
	NotificationRequest request = notification->request();
	
}


}
}
