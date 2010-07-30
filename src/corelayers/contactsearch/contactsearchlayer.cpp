/****************************************************************************
 *  contactsearchlayer.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "contactsearchlayer.h"
#include "libqutim/contact.h"
#include "libqutim/icon.h"
#include "src/modulemanagerimpl.h"

namespace Core
{

static Core::CoreModuleHelper<ContactSearch> contact_search_static(
		QT_TRANSLATE_NOOP("Plugin", "Contact search"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of contact search window")
);

ContactSearch::ContactSearch()
{
	QObject *contactList = getService("ContactList");
	if (contactList) {
		static QScopedPointer<ActionGenerator> button(new ActionGenerator(Icon("edit-find-contact"),
										QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
										this, SLOT(show(QObject*))));
		//QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, button.data()));
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		if (controller)
			controller->addAction(button.data());
	}
}

void ContactSearch::show(QObject*)
{
	if (searchContactForm) {
		searchContactForm->raise();
	} else {
		searchContactForm = new ContactSearchForm();
		centerizeWidget(searchContactForm);
		searchContactForm->show();
		searchContactForm->setAttribute(Qt::WA_DeleteOnClose, true);
	}
}

}
