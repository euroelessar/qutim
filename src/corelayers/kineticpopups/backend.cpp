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
#include "settings/popupbehavior.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace KineticPopups
{

	static Core::CoreModuleHelper<Backend> kinetic_popup_static(
			QT_TRANSLATE_NOOP("Plugin", "Kinetic popups"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM popup realization. Powered by Kinetic")
			);


	Backend::Backend ()
	{
	}

	void Backend::show(Notifications::Type type, QObject* sender, const QString& body,
					   const QString& customTitle)
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
		QString title = customTitle.isEmpty() ? Notifications::toString(type).arg(sender_name) : customTitle;
		QString popup_id = title;

		bool updateMode = manager->updateMode;
		bool appendMode = manager->appendMode;

		Popup *popup = manager->getById(popup_id);
		if (popup)
		{
			if (appendMode) {
				updateMode ? popup->updateMessage(text) : popup->appendMessage(text);
				return;
			}
			else if (sender) {
				popup_id.append("." + QString::number(id_counter++));
			}
		}
		popup  = new Popup ();		
		popup->setTimeOut(manager->timeout);
		popup->setMessage(title,text,sender);
		popup->setId(popup_id);
		if (sender)
			connect(sender,SIGNAL(destroyed(QObject*)),popup,SLOT(deleteLater()));
		popup->send();
	}

}
