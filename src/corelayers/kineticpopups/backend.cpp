/****************************************************************************
 *  backend.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "libqutim/settingslayer.h"
#include "popupwidget.h"
#include "popupappearance.h"
#include "libqutim/icon.h"
#include "popupbehavior.h"

namespace KineticPopups
{

	static Core::CoreModuleHelper<Backend> kinetic_popup_static(
			QT_TRANSLATE_NOOP("Plugin", "Kinetic popups"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM popup realization. Powered by Kinetic")
			);


	Backend::Backend ()
	{
		GeneralSettingsItem<PopupAppearance> *appearance = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance, Icon("dialog-information"), QT_TRANSLATE_NOOP("Settings","Popups"));
		GeneralSettingsItem<PopupBehavior> *behavior = new GeneralSettingsItem<PopupBehavior>(Settings::General,Icon("dialog-information"),QT_TRANSLATE_NOOP("Settings","Notifications"));
		Settings::registerItem(appearance);
		Settings::registerItem(behavior);
	}

	void Backend::show(Notifications::Type type, QObject* sender, const QString& body, const QString& customTitle)
	{
		Manager *manager =  Manager::self();
 		if (!(manager->showFlags & type))
 		{
			return;
		}
		static int id_counter = 0;
		QString sender_id = sender ? sender->property("id").toString() : QString();
		QString sender_name = sender ? sender->property("name").toString() : QString();
		if(sender_name.isEmpty())
			sender_name = sender_id;
		QString popup_id = sender_id.isEmpty() ? QString::number(id_counter++) : sender_id;
		QString title = getTitle(type, popup_id, sender_name);
		QString image_path = sender ? sender->property("avatar").toString() : QString();
		bool updateMode = sender ? sender->property("updateMode").toBool() : false;
		if(image_path.isEmpty())
			image_path = QLatin1String(":/icons/qutim_64");
		Popup *popup = manager->getById(popup_id);
		if (popup)
		{
			if (manager->appendMode) {
				updateMode ? popup->updateMessage(body) : popup->appendMessage(body);
				return;
			}
			else if (sender)
			{
				popup_id.append("." + QString::number(id_counter++));
			}
		}
		popup  = new Popup ();
		popup->setId(popup_id);
		popup->setTimeOut(manager->timeout);
		popup->setMessage(customTitle.isEmpty() ? title : customTitle,body,image_path);
		popup->setSender(sender);
		if (sender)
			connect(sender,SIGNAL(destroyed(QObject*)),popup,SLOT(deleteLater()));
		popup->send();
	}

}
