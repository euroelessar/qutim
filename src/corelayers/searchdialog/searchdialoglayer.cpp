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
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/servicemanager.h>

namespace Core
{
	SearchLayer::SearchLayer()
	{
		foreach(const ObjectGenerator *gen, ObjectGenerator::module<ContactSearchFactory>())
			m_contactSearchFactories << gen->generate<ContactSearchFactory>();
		QObject *contactList = ServiceManager::getByName("ContactList");
		if (contactList) {
			static ActionGenerator button(Icon("edit-find-contact"),
										  QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
										  this,
										  SLOT(showContactSearch(QObject*)));
			//QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, button.data()));
			button.addHandler(ActionVisibilityChangedHandler,this);
			MenuController *controller = qobject_cast<MenuController*>(contactList);
			if (controller)
				controller->addAction(&button);
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

	bool SearchLayer::event(QEvent *ev)
	{
		if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
			ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
			if (event->isVisible()) {
				bool enabled = false;
				foreach (AbstractSearchFactory *factory, m_contactSearchFactories) {
					enabled = !factory->requestList().isEmpty();
					if (enabled)
						break;
				}
				event->action()->setEnabled(enabled);
				event->accept();
			} else {
				event->ignore();
			}
		}
		return QObject::event(ev);
	}

}
