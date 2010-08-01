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
#include "libqutim/config.h"

namespace Core
{

static Core::CoreModuleHelper<SearchLayer> search_dialog_static(
		QT_TRANSLATE_NOOP("Plugin", "Search window"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of search window")
);

SearchLayer::SearchLayer()
{
	foreach(const ObjectGenerator *gen, moduleGenerators<ContactSearchFactory>())
		m_contactSearchFactories << gen->generate<ContactSearchFactory>();
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
	delete m_contactSearchDialog;
	qDeleteAll(m_contactSearchFactories);
}

void SearchLayer::showContactSearch(QObject*)
{
	AbstractSearchForm *form = m_contactSearchDialog.data();
	if (!form) {
		AbstractSearchFormFactory *f = AbstractSearchFormFactory::instance();
		if (f)
			form = f->createForm(m_contactSearchFactories,
								 QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
								 Icon("edit-find-contact"));
		if (!form)
			return;
		centerizeWidget(form);
		form->show();
		form->setAttribute(Qt::WA_DeleteOnClose, true);
		m_contactSearchDialog = form;
	} else {
		form->raise();
	}
}

QWidget *SearchLayer::createSearchDialog(const QList<AbstractSearchFactory*> &factories,
										 const QString &title,
										 const QIcon &icon,
										 QWidget *parent)
{
	AbstractSearchFormFactory *f = AbstractSearchFormFactory::instance();
	return f ? f->createForm(factories, title, icon, parent) : 0;
}

}
