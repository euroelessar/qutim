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
#include <qutim/notification.h>
#include <qutim/chatsession.h>
#include <qutim/systeminfo.h>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextDocument>
#include <QSslError>
#include "rostermanager.h"

#define LOGIN_URL (QLatin1String("api/login"))
#define LOGOUT_URL (QLatin1String("api/logout"))
#define GET_ANSWERS_URL (QLatin1String("api/getAnswers"))
#define GET_ACCOUNTS_URL (QLatin1String("api/getAccounts"))
#define MODIFY_ROSTER_URL (QLatin1String("api/modifyRoster"))
#define APPEND_MESSAGE_URL (QLatin1String("api/appendMessages"))

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

static quint64 encryptedMessageId;
static bool encryptedMessageIdInited = false;

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
	clear();
}

void ActionList::clear()
{
	Action *action = m_first;
	while (action) {
		Action *next = action->next;
		delete action;
		action = next;
	}
	m_first = m_last = 0;
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
    QNetworkAccessManager(parent), m_answersReply(0), m_currentReply(0)
{
	connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(onReplyFinished(QNetworkReply*)));
	QString file = SystemInfo::getDir(SystemInfo::SystemShareDir).filePath("certs/control.pem");
	m_certificate = QSslCertificate::fromPath(file).value(0);
	if (!m_certificate.isValid()) {
		NotificationRequest request(Notification::System);
		request.setTitle(tr("Control plugin"));
		request.setText(tr("Put server's certificate at \"%1\"")
		                .arg(file));
		request.send();
	}
}

void NetworkManager::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId())
		onTimer();
	else
		QNetworkAccessManager::timerEvent(event);
}

void NetworkManager::loadSettings(bool init, bool *changed)
{
	Config config("control");
	config.beginGroup("general");
	QString username = config.value("username", QString());
	QUrl base = QUrl::fromUserInput(config.value("url", QString()));
	m_localAnswers = config.value("answers", QStringList());
	rebuildAnswers();
	if (username != m_username || base != m_base)
		*changed = true;
	else
		*changed = false;
	m_username = username;
	m_base = base;
	if (init)
		loadActions();
}

void NetworkManager::clearQueue()
{
	m_actions.clear();
	delete m_currentReply;
	m_currentReply = 0;
}

QStringList NetworkManager::answers() const
{
	return m_answers;
}

void NetworkManager::addAccount(Account *account)
{
	addAccount(account->protocol()->id(), account->id());
}

void NetworkManager::addAccount(const QString &protocol, const QString &id)
{
	AccountAction *action = new AccountAction(Action::AddAccount);
	action->account = AccountId(id, protocol);
	m_actions << action;
	trySend();
}

void NetworkManager::removeAccount(Account *account)
{
	removeAccount(account->protocol()->id(), account->id());
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
	ChatUnit *contact = const_cast<ChatUnit*>(message.chatUnit()->getHistoryUnit());
	action->account = AccountId(contact->account());
	action->contact = contact->id();
	action->time = message.time();
	action->text = message.text();
	action->incoming = message.isIncoming();
	if (message.property("otrEncrypted", false))
		action->encryption << QLatin1String("otr");
	if (message.property("pgpEncrypted", false)
	        || (encryptedMessageIdInited
	            && encryptedMessageId == message.id())) {
		action->encryption << QLatin1String("pgp");
	}
	m_actions << action;
	trySend();
}

void NetworkManager::sendRequest(Contact *contact, const QString &text)
{
	Config config("control");
	config.beginGroup("general");
	QUrl url = QUrl::fromUserInput(config.value("requestUrl", QString()));
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
	QNetworkReply *reply = QNetworkAccessManager::post(request, text.toUtf8());
	connect(contact, SIGNAL(destroyed()), reply, SLOT(deleteLater()));
	reply->setProperty("__control_contact", qVariantFromValue(contact));
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
	if (reply->error() != QNetworkReply::NoError
	        && reply->error() != QNetworkReply::AuthenticationRequiredError) {
		NotificationRequest request(Notification::System);
		request.setTitle(tr("Control plugin"));
		request.setText(tr("Error connecting to %1:\n%2")
		                .arg(reply->request().url().toString())
		                .arg(reply->errorString()));
		request.send();
	}
	const QByteArray readData = reply->readAll();
	if (reply == m_answersReply) {
		m_answersReply = 0;
		if (reply->error() == QNetworkReply::NoError) {
			QVariantMap data = Json::parse(readData).toMap();
			qDebug() << data << data.value("success");
			if (data.value("success").toBool()) {
				m_networkAnswers = data.value("body").toStringList();
				rebuildAnswers();
			}
		} else {
			if (!m_timer.isActive())
				m_timer.start(60000, this);
		}
		return;
	} else if (Contact *contact = reply->property("__control_contact").value<Contact*>()) {
		if (reply->error() == QNetworkReply::NoError) {
			ChatSession *session = ChatLayer::get(contact, true);
			QString text = QString::fromUtf8(readData);
			QMetaObject::invokeMethod(ChatLayer::instance(),
			                          "insertText",
			                          Q_ARG(qutim_sdk_0_3::ChatSession*, session),
			                          Q_ARG(QString, text));
			NotificationRequest request(Notification::System);
			request.setTitle(tr("Control plugin"));
			request.setText(tr("Received reply from server:\n%1").arg(text));
			request.send();
		}
		return;
	}
	Q_ASSERT(reply == m_currentReply);
	m_currentReply = 0;
	qDebug() << reply->error() << reply->errorString() << readData;
	if (reply->request().url().path().endsWith(LOGIN_URL)) {
		if (reply->error() != QNetworkReply::NoError) {
			NotificationRequest request(Notification::System);
			request.setTitle(tr("Control plugin"));
			request.setText(tr("Invalid login or/and password. Please set right one in settings"));
			request.send();
		} else {
			trySend();
		}
	} else {
		Scope::Ptr scope = reply->property("scope").value<Scope::Ptr>();
		Q_ASSERT(scope);
		if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
			m_actions.prepend(scope->actions);
			QVariantMap data;
			Config config("control");
			config.beginGroup("general");
			data.insert("username", m_username);
			data.insert("password", config.value("password", QString(), Config::Crypted));
			data.insert("remember", true);
			QUrl url = m_base.resolved(QUrl(LOGIN_URL));
			QNetworkReply *reply = post(url, Json::generate(data));
			m_currentReply = reply;
		} else if (reply->error() == QNetworkReply::NoError) {
			QVariantMap data = Json::parse(readData).toMap();
			qDebug() << data << data.value("success");
			if (data.value("success").toBool()) {
				QVariantMap body = data.value("body").toMap();
				int accountId = body.value("accountId", -1).toInt();
				RosterManager::instance()->setAccountId(scope->account.protocol, scope->account.id, accountId);
			}
			trySend();
		} else {
			if (!m_timer.isActive())
				m_timer.start(60000, this);
			m_actions.prepend(scope->actions);
		}
	}
}

static const char *actionTypes[] = {
    "add",
    "update",
    "remove"
};

static Action *loadAction(Config &config, int index)
{
	Action *action = NULL;
	config.setArrayIndex(index);
	Action::Type type = config.value("type", Action::TypesCount);
	switch (type) {
	case Action::AddAccount:
	case Action::RemoveAccount:
		action = new AccountAction(type);
		break;
	case Action::AddContact:
	case Action::UpdateContact:
	case Action::RemoveContact:
		action = new ContactAction(type);
		static_cast<ContactAction*>(action)->id = config.value("id", QString());
		static_cast<ContactAction*>(action)->name = config.value("name", QString());
		static_cast<ContactAction*>(action)->groups = config.value("groups", QStringList());
		break;
	case Action::Message:
		action = new MessageAction();
		static_cast<MessageAction*>(action)->contact = config.value("contact", QString());
		static_cast<MessageAction*>(action)->time = config.value("time", QDateTime());
		static_cast<MessageAction*>(action)->text = config.value("text", QString());
		static_cast<MessageAction*>(action)->incoming = config.value("incoming", false);
		static_cast<MessageAction*>(action)->encryption = config.value("encryption", QStringList());
		break;
	default:
		break;
	}
	action->account.id = config.value("accountId", QString());
	action->account.protocol = config.value("protocol", QString());
	return action;
}

static void storeAction(Config &config, int index, Action *action)
{
	config.setArrayIndex(index);
	config.setValue("accountId", action->account.id);
	config.setValue("protocol", action->account.protocol);
	config.setValue("type", action->type);
	switch (action->type) {
	case Action::AddAccount:
	case Action::RemoveAccount:
		break;
	case Action::AddContact:
	case Action::UpdateContact:
	case Action::RemoveContact:
		config.setValue("id", static_cast<ContactAction*>(action)->id);
		config.setValue("name", static_cast<ContactAction*>(action)->name);
		config.setValue("groups", static_cast<ContactAction*>(action)->groups);
		break;
	case Action::Message:
		config.setValue("contact", static_cast<MessageAction*>(action)->contact);
		config.setValue("time", static_cast<MessageAction*>(action)->time);
		config.setValue("text", static_cast<MessageAction*>(action)->text);
		config.setValue("incoming", static_cast<MessageAction*>(action)->incoming);
		config.setValue("encryption", static_cast<MessageAction*>(action)->encryption);
		break;
	default:
		break;
	}
}

void NetworkManager::trySend()
{
	if (!m_timer.isActive() && !m_currentReply)
		m_timer.start(0, this);
	storeActions();
}

void NetworkManager::onMessageEncrypted(quint64 id)
{
	encryptedMessageIdInited = true;
	encryptedMessageId = id;
}

void NetworkManager::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
	foreach (const QSslError &error, errors) {
		if (error.certificate() == m_certificate) {
			reply->ignoreSslErrors();
			return;
		}
	}
}

void NetworkManager::rebuildAnswers()
{
	QStringList answers = m_networkAnswers + m_localAnswers;
	if (answers != m_answers) {
		m_answers = answers;
		emit answersChanged(m_answers);
	}
}

void NetworkManager::loadActions()
{
	Config cache("controlCache");
	int size = cache.beginArray("actions");
	for (int i = 0; i < size; ++i) {
		if (Action *action = loadAction(cache, i))
			m_actions.append(action);
	}
	trySend();
}

void NetworkManager::storeActions()
{
	Config cache("controlCache");
	int oldSize = cache.beginArray("actions");
	int size = 0;
	if (m_currentReply) {
		Scope::Ptr scope = m_currentReply->property("scope").value<Scope::Ptr>();
		if (scope) {
			Action *action = scope->actions.first();
			while (action) {
				storeAction(cache, size, action);
				++size;
				action = action->next;
			}
		}
	}
	Action *action = m_actions.first();
	while (action) {
		storeAction(cache, size, action);
		++size;
		action = action->next;
	}
	for (int i = oldSize - 1; i >= size; --i)
		cache.remove(i);
}
	
void NetworkManager::onTimer()
{
	m_timer.stop();
	if (m_networkAnswers.isEmpty() && !m_answersReply)
		m_answersReply = get(m_base.resolved(QUrl(GET_ANSWERS_URL)));
	if (m_actions.isEmpty())
		return;
	bool messages = false;
	ActionList actions;
	AccountId account;
	Action *action = m_actions.first();
	while (action) {
		Action *next = action->next;
		if (actions.isEmpty() || (messages && action->type != Action::Message)) {
			m_actions.prepend(actions);
			account = action->account;
			messages = (action->type == Action::Message);
		}
		if (messages || action->account == account) {
			m_actions.remove(action);
			actions.append(action);
		}
		action = next;
	}
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
			data.insert("encryption", message->encryption);
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
	m_currentReply = reply;
}

} // namespace Control
