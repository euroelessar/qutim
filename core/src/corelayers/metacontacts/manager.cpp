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

#include "manager.h"
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include "mergedialog.h"
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>
#include "factory.h"
#include <qutim/profile.h>
#include <QTimer>
#include <QCoreApplication>

namespace Core
{
namespace MetaContacts
{
using namespace qutim_sdk_0_3;

Manager::Manager() : 
	m_storage(RosterStorage::instance()),
	m_factory(new Factory(this)),
	m_blockUpdate(false)
{
	connect(this, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)), SLOT(onContactCreated(qutim_sdk_0_3::Contact*)));
	QTimer::singleShot(0, this, SLOT(initActions()));
	setContactsFactory(m_factory.data());
	m_handler.reset(new MetaContactMessageHandler);
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler.data(),
	                                               QLatin1String("MetaContact"),
	                                               qutim_sdk_0_3::MessageHandler::HighPriority,
	                                               qutim_sdk_0_3::MessageHandler::HighPriority);
	//TODO implement logic
	m_name = Profile::instance()->value("name").toString();
	if (m_name.isEmpty())
		m_name = QCoreApplication::translate("Metacontact", "You");
}

Manager::~Manager()
{
}

ChatUnit *Manager::getUnit(const QString &unitId, bool create)
{
	MetaContactImpl *contact = m_contacts.value(unitId);
	if (!contact && create) {
		contact = new MetaContactImpl(unitId);
		m_contacts.insert(unitId, contact);
		emit contactCreated(contact);
	}
	return contact;
}

void Manager::loadContacts()
{
	m_blockUpdate = true;
	m_storage->load(this);
	m_blockUpdate = false;
}

void Manager::onSplitTriggered(QObject *object)
{
	//TODO implement logic
	MetaContactImpl *metaContact = qobject_cast<MetaContactImpl*>(object);
	foreach (Contact *c, metaContact->contacts()) {
		metaContact->removeContact(c);
	}
}

void Manager::onCreateTriggered(QObject *obj)
{
	MergeDialog *dialog = new MergeDialog;
	if(MetaContactImpl *m = qobject_cast<MetaContactImpl*>(obj))
		dialog->setMetaContact(m);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

QString Manager::name() const
{
	return m_name;
}

void Manager::onContactCreated(qutim_sdk_0_3::Contact *contact)
{
	if(!m_blockUpdate) {
		m_storage->addContact(contact);
	}
}

void Manager::initActions()
{
	if (MenuController *cl = ServiceManager::getByName<MenuController*>("ContactList")) {	
		ActionGenerator *gen = new ActionGenerator(Icon("list-remove-user"),
												QT_TRANSLATE_NOOP("MetaContact","Split Metacontact"),
												this,
												SLOT(onSplitTriggered(QObject*)));
		gen->setType(ActionTypeContactList);
		MenuController::addAction<MetaContactImpl>(gen);
		gen = new ActionGenerator(Icon("list-add-user"),
								QT_TRANSLATE_NOOP("MetaContact","Manage metacontacts"),
								this,
								SLOT(onCreateTriggered(QObject*)));
		gen->setType(ActionTypeContactList);
		MenuController::addAction<MetaContactImpl>(gen);
		cl->addAction(gen);
	}
}


}
}

