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
#include "autoreplybuttonaction.h"
#include "quickanswerbuttonaction.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <qutim/notification.h>
#include <qutim/icon.h>
#include <QStringBuilder>
#include <QSet>

#define NONGROUP_MODEL_ID "Core::SimpleContactList::NonGroupModel"
#define DEFAULT_MODEL_ID "Core::SimpleContactList::TreeModel"

namespace Control {

using namespace qutim_sdk_0_3;

struct MessageListContainer {
	typedef QSharedPointer<MessageListContainer> Ptr;
	MessageList messages;
};

}

Q_DECLARE_METATYPE(Control::MessageListContainer::Ptr)

namespace Control {

RosterManager *RosterManager::self = NULL;

static Config accountConfig(Account *account)
{
	Config config(QLatin1String("control"));
	config.beginGroup(QLatin1String("accounts"));
	config.beginGroup(account->protocol()->id());
	config.beginGroup(account->id());
	return config;
}

RosterManager::RosterManager()
{
	Q_ASSERT(!self);
	self = this;
	m_manager = new NetworkManager(this);
	m_autoReplyGenerator.reset(new AutoReplyButtonActionGenerator(this, SLOT(onAutoReplyClicked(QAction*,QObject*))));
	m_quickAnswerGenerator.reset(new QuickAnswerButtonActionGenerator(this, SLOT(onQuickAnswerClicked(QObject*))));
	//QT_TRANSLATE_NOOP("RosterManager", "Show groups")
	m_groupGenerator.reset(new ActionGenerator(Icon("view-group"), "Show groups",
	                                           this, SLOT(onGroupsClicked(QAction*))));
	ServicePointer<QObject> form("ChatForm");
	if (form) {
		QMetaObject::invokeMethod(form, "addAction", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_autoReplyGenerator.data()));
		QMetaObject::invokeMethod(form, "addAction", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_quickAnswerGenerator.data()));
	}
	ServicePointer<QObject> contactList("ContactList");
	if (contactList) {
		QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_groupGenerator.data()));
	}
	m_settingsItem = new GeneralSettingsItem<Control::SettingsWidget>(
	                     Settings::Plugin,	QIcon(),
						 QT_TRANSLATE_NOOP("Plugin", "Control"));
	m_settingsItem->connect(SIGNAL(saved()), this, SLOT(loadSettings()));
	Settings::registerItem(m_settingsItem);
	Event::eventManager()->installEventFilter(this);
	MessageHandler::registerHandler(this, "Control", ChatInPriority - 1, ChatOutPriority - 1);
	loadSettings(true);
	onStarted();
}

RosterManager::~RosterManager()
{
	ServicePointer<QObject> form("ChatForm");
	if (form) {
		QMetaObject::invokeMethod(form, "removeAction", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_autoReplyGenerator.data()));
		QMetaObject::invokeMethod(form, "removeAction", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_quickAnswerGenerator.data()));
	}
	ServicePointer<QObject> contactList("ContactList");
	if (contactList) {
		QMetaObject::invokeMethod(contactList, "removeButton", Q_ARG(qutim_sdk_0_3::ActionGenerator*, m_groupGenerator.data()));
	}
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
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
	if (!proto || pid < 0)
		return;
	Account *account = proto->account(id);
	if (m_contexts.contains(account)) {
		AccountContext &context = m_contexts[account];
		context.id = pid;
		context.created = true;
		Config cfg = accountConfig(account);
		cfg.setValue("id", pid);
		cfg.setValue("created", context.created);
	}
}

void RosterManager::loadSettings(bool init)
{
	bool changed = false;
	m_manager->loadSettings(init, &changed);
	if (changed && !init) {
		m_manager->clearQueue();
		{
			Config config("control");
			config.remove("accounts");
		}
		foreach (Account *account, m_contexts.keys()) {
			m_manager->addAccount(account);
			foreach (Contact *contact, account->findChildren<Contact*>()) {
				if (contact->isInList())
					m_manager->addContact(contact);
			}
		}
	}
}

void RosterManager::onAccountCreated(Account *account)
{
	if (account->inherits("Jabber::JAccount")) {
		connect(account, SIGNAL(messageEcnrypted(quint64)),
		        m_manager, SLOT(onMessageEncrypted(quint64)));
	}
	connectAccount(account);
	AccountContext &context = m_contexts[account];
	const QString idName = QLatin1String("id");
	Config cfg = accountConfig(account);
	context.id = cfg.value(idName, -1);
	context.created = cfg.value("created", false);
	const bool newAccount = !context.created;
	if (newAccount)
		m_manager->addAccount(account->protocol()->id(), account->id());
	foreach (Contact *contact, account->findChildren<Contact*>()) {
		if (newAccount && contact->isInList())
			onContactCreated(contact);
		else
			connectContact(contact);
	}
}

void RosterManager::onAccountRemoved(Account *account)
{
	AccountContext &context = m_contexts[account];
	context.created = false;
	Config cfg = accountConfig(account);
	cfg.setValue("created", context.created);
	m_manager->removeAccount(account);
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

static QObject *activeContact()
{
	foreach (ChatSession *session, ChatLayer::instance()->sessions()) {
		if (session->isActive())
			return session->unit();
	}
	return 0;
}

void RosterManager::onAutoReplyClicked(QAction *action, QObject *object)
{
	object = activeContact();
	debug() << Q_FUNC_INFO << action << object;
	ChatUnit *contact = qobject_cast<ChatUnit*>(object);
	Q_ASSERT(contact);
	ChatSession *session = ChatLayer::get(contact, false);
	QString text;
	if (session)
		QMetaObject::invokeMethod(session, "quote", Q_RETURN_ARG(QString, text));
	if (text.isEmpty()) {
		const int count = action->property("__control_count").toInt();
		const MessageList messages = RosterManager::messages(contact);
		const int first = qMax(0, messages.size() - count);
		for (int i = first; i < messages.size(); ++i) {
			const Message &message = messages.at(i);
			text += message.text();
			text += QLatin1Char('\n');
		}
		text.chop(1);
	}
	NotificationRequest request(Notification::System);
	request.setTitle(tr("Control plugin"));
	request.setText(tr("qutIM sends request to server:\n%1").arg(text));
	request.send();
	m_manager->sendRequest(contact, text);
}

void RosterManager::onQuickAnswerClicked(QObject *object)
{
	object = activeContact();
	debug() << Q_FUNC_INFO << object;
	ChatUnit *contact = qobject_cast<ChatUnit*>(object);
	if (contact)
		new QuickAnswerMenu(contact);
}

void RosterManager::onGroupsClicked(QAction *action)
{
	Q_UNUSED(action);
	ServicePointer<QObject> model("ContactModel");
	if (model) {
		const char *modelId = model->metaObject()->className();
		bool isNonGroup = !qstrcmp(modelId, NONGROUP_MODEL_ID);
		Config config("control");
		config.beginGroup("groupsButton");
		QByteArray newModelId;
		if (isNonGroup) {
			newModelId = config.value("model", QLatin1String(DEFAULT_MODEL_ID)).toLatin1();
		} else {
			config.setValue("model", QLatin1String(modelId));
			newModelId = NONGROUP_MODEL_ID;
		}
		ExtensionInfoList list = ServiceManager::instance()->listImplementations("ContactModel");
		foreach (const ExtensionInfo &info, list) {
			if (info.generator()->metaObject()->className() == newModelId) {
				ServiceManager::instance()->setImplementation("ContactModel", info);
				return;
			}
		}
	}
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
	if (message.property("service", false))
		return Accept;
	if (ChatSession *session = ChatLayer::get(message.chatUnit(), false)) {
		MessageListContainer::Ptr container = session->property("__control_container").value<MessageListContainer::Ptr>();
		if (!container) {
			container = MessageListContainer::Ptr::create();
			session->setProperty("__control_container", qVariantFromValue(container));
		}
		if (message.isIncoming())
			container->messages << message;
	}
	if (!message.property("history", false))
		m_manager->sendMessage(message);
	return Accept;
}

MessageList RosterManager::messages(ChatUnit *unit)
{
	if (ChatSession *session = ChatLayer::get(unit, false)) {
		MessageListContainer::Ptr container = session->property("__control_container").value<MessageListContainer::Ptr>();
		if (container)
			return container->messages;
	}
	return MessageList();
}

NetworkManager *RosterManager::networkManager()
{
	return m_manager;
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
				config.beginGroup(id);
				config.setValue("created", false);
				config.endGroup();
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
