/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "floaties.h"
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/servicemanager.h>
#include <qutim/event.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QTime>

void FloatiesPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Floaties"),
			QT_TRANSLATE_NOOP("Plugin", "Implementation of floaty contacts"),
			PLUGIN_VERSION(0, 2, 0, 0),
			ExtensionIcon("bookmark-new"));
	setCapabilities(Loadable);
	m_eventId = Event::registerType("contact-list-drop");
	m_model = 0;
//	m_action = new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Floaties", "Remove from Desktop"),
//								   this, SLOT(onRemoveContact(QObject*)));
}

bool FloatiesPlugin::load()
{
	if (m_model)
		return false;
	if (!m_view) {
		if (QObject *contactList = ServiceManager::getByName("ContactList")) {
			QWidget *widget = contactList->property("widget").value<QWidget*>();
			m_view = widget->findChild<QAbstractItemView*>();
		}
	}
	if (!m_view)
		return false;
	m_model = new FloatiesItemModel(this);
	
	Config cfg;
	cfg.beginGroup("floaties");
	int size = cfg.beginArray("entities");
	for (int i = 0; i < size; i++) {
		cfg.setArrayIndex(i);
		Protocol *proto = Protocol::all().value(cfg.value("protocol", QString()));
		if (!proto)
			continue;
		Account *account = proto->account(cfg.value("account", QString()));
		if (!account)
			continue;
		ChatUnit *unit = account->unit(cfg.value("id", QString()), true);
		Contact *contact = qobject_cast<Contact*>(unit);
		if (!contact || m_contacts.contains(contact))
			continue;
		ContactWidget *widget = createWidget(contact);
		widget->restoreGeometry(cfg.value("geometry", QByteArray()));
		widget->show();
	}
	
	Event::eventManager()->installEventFilter(this);
	return true;
}

bool FloatiesPlugin::unload()
{
	if (!m_model)
		return false;
	Event::eventManager()->removeEventFilter(this);
	Config cfg;
	cfg.beginGroup("floaties");
	int size = cfg.beginArray("entities");
	int i;
	QMap<Contact*, ContactWidget*>::iterator it = m_contacts.begin();
	for (i = 0; it != m_contacts.end(); it++, i++) {
		cfg.setArrayIndex(i);
		Contact *contact = it.key();
		cfg.setValue("protocol", contact->account()->protocol()->id());
		cfg.setValue("account", contact->account()->id());
		cfg.setValue("id", contact->id());
		cfg.setValue("geometry", it.value()->saveGeometry());
	}
	for (; size > i; size--)
		cfg.remove(size - 1);
	qDeleteAll(m_contacts);
	m_contacts.clear();
	delete m_model;
	m_model = 0;
	return true;
}

bool FloatiesPlugin::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == Event::eventType()) {
		Event *ev = static_cast<Event*>(event);
		if (ev->id == m_eventId) {
			QPoint pos = ev->at<QPoint>(0);
			Contact *contact = ev->at<Contact*>(1);
			if (contact && !m_contacts.contains(contact)) {
				ContactWidget *widget = createWidget(contact);
				widget->move(pos);
				widget->show();
			}
		}
	}
	return Plugin::eventFilter(obj, event);
}

void FloatiesPlugin::onRemoveContact(QObject *obj)
{
	bool deleted = !qobject_cast<Contact*>(obj);
	Contact *contact = static_cast<Contact*>(obj);
	ContactWidget *widget = m_contacts.take(contact);
	m_model->removeContact(contact);
	if (!deleted && widget) {
		widget->deleteLater();
//		contact->removeAction(m_action);
	}
}

ContactWidget *FloatiesPlugin::createWidget(qutim_sdk_0_3::Contact *contact)
{
	QPersistentModelIndex index = m_model->addContact(contact);
	ContactWidget *widget = new ContactWidget(index, m_view, contact);
	connect(widget, SIGNAL(wantDie(QObject*)), this, SLOT(onRemoveContact(QObject*)));
	m_contacts.insert(contact, widget);
//	contact->addAction(m_action);
	return widget;
}

QUTIM_EXPORT_PLUGIN(FloatiesPlugin)

