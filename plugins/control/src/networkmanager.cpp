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

#include "networkmanager.h"
#include <qutim/protocol.h>
#include <qutim/message.h>
#include <qutim/json.h>
#include <qutim/servicemanager.h>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "rostermanager.h"

#define LOGIN_URL (QLatin1String("api/login"))
#define LOGOUT_URL (QLatin1String("api/logout"))
#define GET_ANSWERS_URL (QLatin1String("api/getAnswers"))
#define GET_ACCOUNTS_URL (QLatin1String("api/getAccounts"))
#define MODIFY_ROSTER_URL (QLatin1String("api/modifyRoster"))
#define APPEND_MESSAGE_URL (QLatin1String("api/appendMessages"))

Q_GLOBAL_STATIC(qutim_sdk_0_3::ServicePointer<Control::RosterManager>, rosterManager)

namespace Control {

struct Scope
{
	typedef QSharedPointer<Scope> Ptr;
	ActionList actions;
	AccountId account;
};

}

Q_DECLARE_METATYPE(Control::Scope::Ptr)

namespace Control {

using namespace qutim_sdk_0_3;

AccountId::AccountId(qutim_sdk_0_3::Account *account)
    : id(account->id()), protocol(account->protocol()->id())
{
}

AccountId::AccountId(const QString &id, const QString &protocol)
    : id(id), protocol(protocol)
{
}

ActionList::ActionList()
    : m_first(0), m_last(0)
{
}

ActionList::~ActionList()
{
	Action *action = m_first;
	while (action) {
		Action *next = action->next;
		delete action;
		action = next;
	}
}

Action *ActionList::first()
{
	return m_first;
}

Action *ActionList::last()
{
	return m_last;
}

bool ActionList::isEmpty() const
{
	return m_first == 0;
}

bool ActionList::hasSingleElement() const
{
	return m_first && m_first == m_last;
}

int ActionList::count() const
{
	Action *action = m_first;
	int result = 0;
	while (action) {
		++result;
		action = action->next;
	}
	return result;
}

void ActionList::append(Action *action)
{
	if (!m_first)
		m_first = action;
	if (m_last)
		m_last->next = action;
	action->prev = m_last;
	m_last = action;
}

void ActionList::append(ActionList &o)
{
	if (o.m_first) {
		append(o.m_first);
		o.m_last = o.m_last;
	}
	o.m_first = o.m_last = 0;
}

void ActionList::prepend(Action *action)
{
	if (!m_last)
		m_last = action;
	if (m_first)
		m_first->prev = action;
	action->next = m_first;
	m_first = action;
}

void ActionList::prepend(ActionList &o)
{
	if (o.m_last) {
		prepend(o.m_last);
		m_first = o.m_first;
	}
	o.m_first = o.m_last = 0;
}

void ActionList::remove(Action *action)
{
	if (action->prev)
		action->prev->next = action->next;
	if (action->next)
		action->next->prev = action->prev;
	if (m_first == action)
		m_first = action->next;
	if (m_last == action)
		m_last = action->prev;
	action->prev = action->next = 0;
}

NetworkManager::NetworkManager(QObject *parent) :
    QNetworkAccessManager(parent), m_current(0)
{
	m_base = QUrl(QLatin1String("http://localhost/"));
	connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(onReplyFinished(QNetworkReply*)));
}

void NetworkManager::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId())
		onTimer();
	else
		QNetworkAccessManager::timerEvent(event);
}

void NetworkManager::addAccount(const QString &protocol, const QString &id)
{
	AccountAction *action = new AccountAction(Action::AddAccount);
	action->account = AccountId(id, protocol);
	m_actions << action;
	trySend();
}

void NetworkManager::removeAccount(const QString &protocol, const QString &id)
{
	AccountAction *action = new AccountAction(Action::RemoveAccount);
	action->account = AccountId(id, protocol);
	m_actions << action;
	trySend();
}

void NetworkManager::addContact(qutim_sdk_0_3::Contact *contact)
{
	m_actions << new ContactAction(Action::AddContact, contact);
	trySend();
}

void NetworkManager::removeContact(qutim_sdk_0_3::Contact *contact)
{
	m_actions << new ContactAction(Action::RemoveContact, contact);
	trySend();
}

void NetworkManager::updateContact(qutim_sdk_0_3::Contact *contact)
{
	m_actions << new ContactAction(Action::UpdateContact, contact);
	trySend();
}

void NetworkManager::sendMessage(const qutim_sdk_0_3::Message &message)
{
	MessageAction *action = new MessageAction();
	ChatUnit *contact = message.chatUnit();
	action->account = AccountId(contact->account());
	action->contact = contact->id();
	action->time = message.time();
	action->text = message.text();
	action->incoming = message.isIncoming();
	m_actions << action;
	trySend();
}

static QByteArray paranoicEscape(const QByteArray &raw)
{
	static char hex[17] = "0123456789ABCDEF";
	QByteArray escaped;
	escaped.reserve(raw.size() * 3);
	for (int i = 0; i < raw.size(); ++i) {
		const quint8 c = static_cast<quint8>(raw[i]);
		escaped += '%';
		escaped += hex[c >> 4];
		escaped += hex[c & 0x0f];
	}
	return escaped;
}

QNetworkReply *NetworkManager::post(const QUrl &url, const QByteArray &body)
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QByteArray data = "body=" + paranoicEscape(body);
	return QNetworkAccessManager::post(request, data);
}

QNetworkReply *NetworkManager::get(const QUrl &url)
{
	QNetworkRequest request(url);
	return QNetworkAccessManager::get(request);
}

void NetworkManager::onReplyFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	Q_ASSERT(reply == m_current);
	m_current = 0;
	QByteArray readData = reply->readAll();
	qDebug() << reply->error() << reply->errorString() << readData;
	if (reply->request().url().path().endsWith(LOGIN_URL)) {
		if (reply->error() != QNetworkReply::NoError) {
			qFatal("Invalid credentials");
		} else {
			m_timer.start(0, this);
		}
	} else {
		Scope::Ptr scope = reply->property("scope").value<Scope::Ptr>();
		Q_ASSERT(scope);
		if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
			m_actions.prepend(scope->actions);
			QVariantMap data;
			Config config("control");
			config.beginGroup("general");
			data.insert("username", config.value("usename", QString()));
			data.insert("password", config.value("password", QString(), Config::Crypted));
			data.insert("remember", true);
			QUrl url = m_base.resolved(QUrl(LOGIN_URL));
			QNetworkReply *reply = post(url, Json::generate(data));
			m_current = reply;
		} else {
			QVariantMap data = Json::parse(readData).toMap();
			qDebug() << data << data.value("success");
			if (data.value("success").toBool()) {
				QVariantMap body = data.value("body").toMap();
				int accountId = body.value("accountId", -1).toInt();
				RosterManager::instance()->setAccountId(scope->account.protocol, scope->account.id, accountId);
			}
			m_timer.start(0, this);
		}
	}
}

static const char *actionTypes[] = {
    "add",
    "update",
    "remove"
};

void NetworkManager::trySend()
{
	if (!m_timer.isActive() && !m_current)
		m_timer.start(0, this);
}
	
void NetworkManager::onTimer()
{
	m_timer.stop();
	if (m_actions.isEmpty())
		return;
	bool messages = false;
	ActionList actions;
	AccountId account;
	Action *action = m_actions.first();
	//qDebug() << m_actions.count() << actions.isEmpty();
	while (action) {
		Action *next = action->next;
		if (actions.isEmpty() || (messages && action->type != Action::Message)) {
			m_actions.prepend(actions);
			account = action->account;
			messages = (action->type == Action::Message);
		}
		//qDebug() << action->account.id << action->account.protocol << account.id << account.protocol;
		if (messages || action->account == account) {
			m_actions.remove(action);
			actions.append(action);
		}
		action = next;
	}
	//qDebug() << m_actions.count() << "+" << actions.count();
	QVariant body;
	if (messages) {
		account.protocol = QString();
		account.id = QString();
		QVariantList messages;
		for (Action *action = actions.first(); action; action = action->next) {
			QVariantMap data;
			MessageAction *message = static_cast<MessageAction*>(action);
			const qint64 time = message->time.toMSecsSinceEpoch() / 1000;
			const int account = RosterManager::instance()
			                    ->accountId(message->account.protocol, message->account.id);
			data.insert("time", time); //message->time.toString("yyyy-MM-dd hh:mm:ss"));
			data.insert("account", account);
			data.insert("contact", message->contact);
			data.insert("message", message->text);
			data.insert("incoming", message->incoming);
			messages << data;
		}
		body = messages;
	} else {
		Action *remove = 0;
		Action *create = 0;
		Action *action = actions.first();
		QVariantList list;
		while (action) {
			Action *next = action->next;
			switch (action->type) {
			case Action::AddAccount:
				if (remove) {
					actions.remove(remove);
					actions.remove(action);
					delete remove;
					delete action;
					remove = 0;
				} else {
					create = action;
				}
				break;
			case Action::RemoveAccount:
				remove = action;
				break;
			case Action::AddContact:
			case Action::RemoveContact:
			case Action::UpdateContact: {
				ContactAction *contact = static_cast<ContactAction*>(action);
				QString type = QLatin1String(actionTypes[action->type - Action::AddContact]);
				QVariantMap data;
				data.insert("type", type);
				data.insert("id", contact->id);
				data.insert("name", contact->name);
				data.insert("groups", contact->groups);
				list << data;
				break;
			}
			default:
				break;
			}
			action = next;
		}
		QString control;
		if (list.isEmpty() && remove) {
			control = "remove";
		} else {
			if (remove) {
				actions.remove(remove);
				m_actions.prepend(remove);
				remove = 0;
			}
			if (create)
				control = "create";
			else
				control = "update";
		}
		QVariantMap data;
		QVariantMap accountData;
		accountData.insert("id", account.id);
		accountData.insert("protocol", account.protocol);
		int pid = RosterManager::instance()->accountId(account.protocol, account.id);
		if (pid >= 0)
			accountData.insert("pid", pid);
		data.insert("actions", list);
		data.insert("account", accountData);
		data.insert("control", control);
		body = data;
	}
	QUrl url = m_base.resolved(QUrl(messages ? APPEND_MESSAGE_URL : MODIFY_ROSTER_URL));
	QNetworkReply *reply = post(url, Json::generate(body));
	Scope::Ptr scope = Scope::Ptr::create();
	scope->actions.prepend(actions);
	scope->account = account;
	reply->setProperty("scope", qVariantFromValue(scope));
	qDebug() << Json::generate(body);
	m_current = reply;
}

} // namespace Control
