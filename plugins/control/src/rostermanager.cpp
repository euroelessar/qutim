/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "rostermanager.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <QStringBuilder>
#include <QSet>

namespace Control {

using namespace qutim_sdk_0_3;

RosterManager *RosterManager::self = NULL;

//static QString accountConfigPath(const QString &protocol, const QString &id)
//{
//	return QLatin1Literal("control/")
//	        % protocol
//	        % QLatin1Char('.')
//	        % id;
//}

//static QString accountConfigPath(Account *account)
//{
//	return accountConfigPath(account->protocol()->id(), account->id());
//}

static Config accountConfig(Account *account)
{
	Config config(QLatin1String("control"));
	config.beginGroup(QLatin1String("accounts"));
	config.beginGroup(account->protocol()->id());
	config.beginGroup(account->id());
	return config;
//	return Config(accountConfigPath(account));
}

RosterManager::RosterManager()
{
	Q_ASSERT(!self);
	self = this;
	m_manager = new NetworkManager(this);
	Event::eventManager()->installEventFilter(this);
	MessageHandler::registerHandler(this, "Control", ChatInPriority - 1, ChatOutPriority - 1);
	onStarted();
}

RosterManager::~RosterManager()
{
	self = 0;
	MessageHandler::unregisterHandler(this);
}

bool RosterManager::event(QEvent *ev)
{
//	static quint16 startupType = Event::registerType("startup");
//	if (ev->type() == Event::eventType()) {
//		Event *event = static_cast<Event*>(ev);
//		if (event->id == startupType) {
//			onStarted();
//			return true;
//		}
//	}
	return QObject::event(ev);
}

RosterManager *RosterManager::instance()
{
	return self;
}

int RosterManager::accountId(const QString &protocol, const QString &id) const
{
	Protocol *proto = Protocol::all().value(protocol);
	if (!proto)
		return -1;
	Account *account = proto->account(id);
	if (m_contexts.contains(account))
		return m_contexts[account].id;
	return -1;
}

void RosterManager::setAccountId(const QString &protocol, const QString &id, int pid)
{
	Protocol *proto = Protocol::all().value(protocol);
	if (!proto)
		return;
	Account *account = proto->account(id);
	if (m_contexts.contains(account)) {
		Config cfg = accountConfig(account);
		cfg.setValue("id", pid);
		m_contexts[account].id = pid;
	}
}

void RosterManager::onAccountCreated(Account *account)
{
	connectAccount(account);
	AccountContext &context = m_contexts[account];
	const QString idName = QLatin1String("id");
	Config cfg = accountConfig(account);
	context.id = cfg.value(idName, -1);
	const bool newAccount = context.id < 0;
	qDebug() << account->id() << context.id << newAccount;
	if (newAccount)
		m_manager->addAccount(account->protocol()->id(), account->id());
	foreach (Contact *contact, account->findChildren<Contact*>()) {
		if(newAccount)
			onContactCreated(contact);
		else
			connectContact(contact);
	}
}

void RosterManager::onContactCreated(Contact *contact)
{
	connectContact(contact);
	m_manager->addContact(contact);
}

void RosterManager::onContactUpdated()
{
	Contact *contact = qobject_cast<Contact*>(sender());
	Q_ASSERT(contact);
	if (!contact->isInList())
		return;
	m_manager->updateContact(contact);
}

void RosterManager::onContactRemoved(Contact *contact)
{
	m_manager->removeContact(contact);
}

void RosterManager::onContactInListChanged(bool inList)
{
	Contact *contact = qobject_cast<Contact*>(sender());
	Q_ASSERT(contact);
	if (inList)
		onContactCreated(contact);
	else
		onContactRemoved(contact);
}

void RosterManager::connectAccount(Account *account)
{
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
	        SLOT(onContactCreated(qutim_sdk_0_3::Contact*)));
}

void RosterManager::connectContact(Contact *contact)
{
	connect(contact, SIGNAL(nameChanged(QString,QString)),
	        SLOT(onContactUpdated()));
	connect(contact, SIGNAL(tagsChanged(QStringList,QStringList)),
	        SLOT(onContactUpdated()));
	connect(contact, SIGNAL(inListChanged(bool)),
	        SLOT(onContactInListChanged(bool)));
}

MessageHandler::Result RosterManager::doHandle(Message &message, QString *reason)
{
	Q_UNUSED(reason);
	m_manager->sendMessage(message);
	return Accept;
}

typedef QPair<QString, QString> PairString;
typedef QSet<PairString> PairStringList;

void RosterManager::onStarted()
{
	Config config("control");
	PairStringList accounts;
	PairStringList oldAccounts;
	foreach (Protocol *protocol, Protocol::all()) {
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		connect(protocol, SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountRemoved(qutim_sdk_0_3::Account*)));
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
	}

	foreach (Account *account, m_contexts.keys()) {
		const PairString pair(account->protocol()->id(), account->id());
		accounts << pair;
	}
	config.beginGroup("accounts");
	foreach (const QString &protocol, config.childGroups()) {
		config.beginGroup(protocol);
		foreach (const QString &id, config.childGroups()) {
			const PairString pair(protocol, id);
			if (!accounts.contains(pair)) {
				m_manager->removeAccount(protocol, id);
//				config.remove(id);
			}
			oldAccounts << pair;
		}
		config.endGroup();
	}
	foreach (const PairString &pair, accounts) {
		if (!oldAccounts.contains(pair))
			m_manager->addAccount(pair.first, pair.second);
	}
}

} // namespace Control
