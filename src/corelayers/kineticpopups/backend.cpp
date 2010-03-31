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
#include "modulemanagerimpl.h"
#include <QVariant>
#include <QTime>
#include <QDebug>
#include "popupwidget.h"
#include <libqutim/configbase.h>
#include "settings/popupappearance.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>
#include <libqutim/message.h>
#include <libqutim/chatunit.h>
#include "libqutim/contactlist.h"
#include "libqutim/contact.h"
#include "libqutim/protocol.h"
#include <QTimer>

namespace KineticPopups
{

	static Core::CoreModuleHelper<Backend> kinetic_popup_static(
			QT_TRANSLATE_NOOP("Plugin", "Kinetic popups"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM popup implementation. Powered by Kinetic")
			);


	Backend::Backend ()
	{
		GeneralSettingsItem<Core::PopupAppearance> *appearance = new GeneralSettingsItem<Core::PopupAppearance>(Settings::Appearance, Icon("dialog-information"), QT_TRANSLATE_NOOP("Settings","Popups"));
		Settings::registerItem(appearance);
		ActionGenerator *action = new ActionGenerator(Icon("roll"), QByteArray(), this, SLOT(onButtonClick()));
		ContactList::instance()->metaObject()->invokeMethod(ContactList::instance(), "addButton", Q_ARG(ActionGenerator*, action));
	}

	void Backend::onButtonClick()
	{
		static QList<Contact*> contacts;
		if (contacts.isEmpty()) {
			QList<Contact*> cList;
			foreach (Protocol *proto, allProtocols()) {
				cList.append(proto->findChildren<Contact*>());
			}
			for (int i = 0; i < 5; i++) {
				contacts << cList.takeAt(qrand() % cList.size());
			}
		}
		static int num = 0;
		Message mess(QString::number(++num, 16));
		mess.setChatUnit(contacts.at(qrand() % contacts.size()));
		mess.setIncoming(true);
		Notifications::sendNotification(mess);
		if (qobject_cast<QAction*>(sender())) {
			QTimer::singleShot(200, this, SLOT(onButtonClick()));
			QTimer::singleShot(700, this, SLOT(onButtonClick()));
			QTimer::singleShot(1200, this, SLOT(onButtonClick()));
		}
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

}
