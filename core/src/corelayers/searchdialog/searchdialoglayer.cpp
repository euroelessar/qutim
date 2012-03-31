/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "searchdialoglayer.h"
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/servicemanager.h>
#include <QApplication>
#include <qutim/systemintegration.h>
#include <qutim/metaobjectbuilder.h>
#include <qutim/protocol.h>

namespace Core
{
SearchLayer::SearchLayer()
{
	// TODO: Think, may be we should move such checks to ModuleManager?
	QList<QLatin1String> protocols;
	foreach (Protocol *protocol, Protocol::all())
		protocols.append(QLatin1String(protocol->metaObject()->className()));

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<ContactSearchFactory>()) {
		const char *proto = MetaObjectBuilder::info(gen->metaObject(), "DependsOn");
		if (!protocols.contains(QLatin1String(proto)))
			continue;
		m_contactSearchFactories << gen->generate<ContactSearchFactory>();
	}
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList) {
        ActionGenerator *gen = new ActionGenerator(Icon("edit-find-contact"),
									  QT_TRANSLATE_NOOP("ContactSearch", "Search contact"),
									  this,
									  SLOT(showContactSearch(QObject*)));
		//QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, button.data()));
        gen->addHandler(ActionVisibilityChangedHandler,this);
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		if (controller)
            controller->addAction(gen);
	}
}

SearchLayer::~SearchLayer()
{
	delete m_contactSearchDialog.data();
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
		form->setParent(QApplication::activeWindow());
#ifdef Q_WS_MAEMO_5
		form->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
		form->setWindowFlags(form->windowFlags() | Qt::Window);
		SystemIntegration::show(form);
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

