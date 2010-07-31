/****************************************************************************
 *  searchdialoglayer.cpp
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

#include "searchdialoglayer.h"
#include "libqutim/contact.h"
#include "libqutim/icon.h"
#include "src/modulemanagerimpl.h"

namespace Core
{

static Core::CoreModuleHelper<SearchLayer> search_dialog_static(
		QT_TRANSLATE_NOOP("Plugin", "Search window"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of search window")
);

SearchLayer::SearchLayer()
{
	QObject *contactList = getService("ContactList");
	if (contactList) {
		static QScopedPointer<ActionGenerator> button(new ActionGenerator(Icon("edit-find-contact"),
										QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
										this, SLOT(showContactSearch(QObject*))));
		//QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, button.data()));
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		if (controller)
			controller->addAction(button.data());
	}
}

SearchLayer::~SearchLayer()
{
	foreach (SearchForm *form, forms)
		delete form;
}

void SearchLayer::showContactSearch(QObject*)
{
	show(const_cast<QMetaObject*>(&ContactSearchFactory::staticMetaObject),
		 QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
		 Icon("edit-find-contact"));
}

QWidget *SearchLayer::show(QMetaObject *factory, const QString &title, const QIcon &icon)
{
	SearchForm *form = forms.value(factory);
	if (form) {
		if (!title.isEmpty())
			form->setWindowTitle(title);
		if (!icon.isNull())
			form->setWindowIcon(icon);
		form->raise();
	} else {
		form = new SearchForm(factory, title, icon);
		centerizeWidget(form);
		form->show();
		form->setAttribute(Qt::WA_DeleteOnClose, true);
		connect(form, SIGNAL(destroyed(QObject*)), SLOT(formDestroyed(QObject*)));
		forms.insert(factory, form);
	}
	return form;
}

void SearchLayer::formDestroyed(QObject *obj)
{
	QHash<QMetaObject*, SearchForm*>::iterator itr = forms.begin();
	QHash<QMetaObject*, SearchForm*>::iterator endItr = forms.end();
	while (itr != endItr) {
		if (*itr == obj) {
			forms.erase(itr);
			break;
		}
		++itr;
	}
}

}
