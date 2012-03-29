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

#include "ircaccount_p.h"
#include "ircprotocol_p.h"
#include "ircconnection.h"
#include "ircchannel.h"
#include "ircchannelparticipant.h"
#include <qutim/status.h>
#include <qutim/chatsession.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <QTime>

namespace qutim_sdk_0_3 {

namespace irc {

QHash<QString, QString> IrcAccountPrivate::logMsgColors;

IrcContact *IrcAccountPrivate::newContact(const QString &nick, const QString &host)
{
	IrcContact *contact = new IrcContact(q, nick, host);
	q->connect(contact, SIGNAL(nameChanged(QString,QString)),
			   SLOT(onContactNickChanged(QString,QString)));
	contacts.insert(nick, contact);
	return contact;
}

IrcAccount::IrcAccount(const QString &network) :
	Account(network, IrcProtocol::instance()), d(new IrcAccountPrivate)
{
	d->q = this;
	d->conn = new IrcConnection(this, this);
	d->groupManager.reset(new IrcGroupChatManager(this));
}

IrcAccount::~IrcAccount()
{
	qDeleteAll(d->channels);
	qDeleteAll(d->contacts);
}

void IrcAccount::setStatus(Status status)
{
	Status current = this->status();
	if (status == Status::Connecting)
		return;
	// Prepare status.
	if (current == Status::Connecting && status != Status::Offline) {
		status.setType(current.text().isEmpty() ? Status::Online : Status::Away);
		status.setText(current.text());
	} else if (status == Status::Offline || status == Status::Online) {
		status.setText(QString());
	} else if (status == Status::Invisible || status == Status::FreeChat) {
		status.setType(Status::Online);
		status.setText(QString());
	} else {
		if (status != Status::Away)
			status.setType(Status::Away);
		if (status.text().isEmpty())
			status.setText(tr("Away"));
	}
	// Send status.
	if (status == Status::Offline) {
		d->conn->disconnectFromHost(false);
		resetGroupChatManager();
	} else {
		if (current == Status::Offline) {
			status = Status::createConnecting(status, "irc");
			d->conn->connectToNetwork();
		} else if (current == Status::Away && status == Status::Online) {
			// It is a little weird but the following command sets status to Online.
			d->conn->send("AWAY");
		}
		if (status.type() == Status::Away)
			d->conn->send(QString("AWAY %1").arg(status.text()));
		if (current == Status::Connecting && status != Status::Offline)
			resetGroupChatManager(d->groupManager.data());
	}
	status.initIcon("irc");
	Account::setStatus(status);
}

QString IrcAccount::name() const
{
	return d->conn->nick();
}

QString IrcAccount::avatar()
{
	return d->avatar;
}

void IrcAccount::setAvatar(const QString &avatar)
{
	d->avatar = avatar;
	emit avatarChanged(avatar);
}

ChatUnit *IrcAccount::getUnitForSession(ChatUnit *unit)
{
	if (IrcChannelParticipant *participant = qobject_cast<IrcChannelParticipant*>(unit)) {
		return participant->contact();
	}
	return unit;
}

ChatUnit *IrcAccount::getUnit(const QString &name, bool create)
{
	Q_UNUSED(create);
	if (name.startsWith('#') || name.startsWith('&'))
		return 0;
	return getContact(name, QString(), false);
}

IrcChannel *IrcAccount::getChannel(const QString &name, bool create)
{
	QString nameLower = name.toLower();
	IrcChannel *channel = d->channels.value(nameLower);
	if (create && !channel) {
		channel = new IrcChannel(this, name);
		d->channels.insert(nameLower, channel);
		emit conferenceCreated(channel);
	}
	return channel;
}

IrcContact *IrcAccount::getContact(const QString &nick, const QString &host, bool create)
{
	IrcContact *contact = d->contacts.value(nick);
	if (create && !contact)
		contact = d->newContact(nick, host);
	if (contact)
		contact->setHostMask(host);
	return contact;
}

IrcContact *IrcAccount::getContact(const QString &nick, bool create)
{
	return getContact(nick, QString(), create);
}

void IrcAccount::removeContact(const QString &id)
{
	d->contacts.remove(id);
}

void IrcAccount::removeChannel(const QString &id)
{
	d->channels.remove(id);
}

void IrcAccount::send(const QString &cmd,
					  bool highPriority,
					  IrcCommandAlias::Type aliasType,
					  const QHash<QChar, QString> &extParams) const
{
	QString command = cmd;
	if (aliasType != IrcCommandAlias::Disabled) {
		bool found;
		QString lastCmdName;
		QString cmdName; // Current command name
		QString cmdParamsStr; // Current parameters
		int i = 0;
		for (; i < 10; ++i) {
			found = false;
			cmdName = command.section(' ', 0, 0);
			if (cmdName.compare(lastCmdName, Qt::CaseInsensitive) == 0) // To avoid recursion
				break;
			cmdParamsStr = command.mid(cmdName.length() + 1);
			QStringList params; // Parameters from the command line
			params << cmdParamsStr;
			params += cmdParamsStr.split(' ', QString::SkipEmptyParts);
			foreach (IrcCommandAlias *alias, IrcProtocolPrivate::aliases) {
				if (cmdName.compare(alias->name(), Qt::CaseInsensitive) == 0) {
					QString error;
					QString newCommand = alias->generate(aliasType, params, extParams, &error);
					if (!error.isEmpty()) {
						log(error, true, "ERROR");
						return;
					} else if (!newCommand.isEmpty()) {
						command = newCommand;
						lastCmdName = cmdName;
						found = true;
					}
					break;
				}
			}
			if (!found)
				break;
		}
		LastCommand lastCmd;
		lastCmd.time = QDateTime::currentDateTime().toTime_t();
		if (i == 0 && !found) { // A suitable alias has not been found
			lastCmd.cmd = cmdName.toUpper();
			command = lastCmd.cmd + " " + cmdParamsStr;
		} else {
			lastCmd.cmd = lastCmdName.toLatin1();
		}
		d->removeOldCommands();
		d->lastCommands.push_back(lastCmd);
	}
	d->conn->send(command, highPriority);
}

void IrcAccount::send(IrcCommandAlias *alias,
					  bool highPriority,
					  IrcCommandAlias::Type aliasType,
					  QStringList params,
					  const ExtendedParams &extParams) const
{
	QString command = alias->generate(aliasType, params, extParams);
	if (command.isEmpty())
		return;
	d->removeOldCommands();
	LastCommand lastCmd = {
		QDateTime::currentDateTime().toTime_t(),
		command.section(' ', 0, 0).toLatin1()
	};
	d->lastCommands.push_back(lastCmd);
	d->conn->send(command, highPriority);
}

void IrcAccount::sendCtpcRequest(const QString &contact, const QString &cmd,
								 const QString &params, bool highPriority)
{
	d->conn->sendCtpcRequest(contact, cmd, params, highPriority);
}

void IrcAccount::sendCtpcReply(const QString &contact, const QString &cmd,
							   const QString &params, bool highPriority)
{
	d->conn->sendCtpcReply(contact, cmd, params, highPriority);
}

void IrcAccount::setName(const QString &name) const
{
	send(QString("NICK %1").arg(name));
}

IrcProtocol *IrcAccount::protocol()
{
	Q_ASSERT(qobject_cast<IrcProtocol*>(Account::protocol()));
	return reinterpret_cast<IrcProtocol*>(Account::protocol());
}

const IrcProtocol *IrcAccount::protocol() const
{
	Q_ASSERT(qobject_cast<const IrcProtocol*>(Account::protocol()));
	return reinterpret_cast<const IrcProtocol*>(Account::protocol());
}

ChatSession *IrcAccount::activeSession() const
{
	ChatSession *session = protocol()->activeSession();
	return session && session->getUnit()->account() == this ? session : 0;
}

void IrcAccount::log(const QString &msg, bool addToActiveSession, const QString &type) const
{
	QString plainText = IrcProtocol::ircFormatToPlainText(msg);
	QString html = IrcProtocol::ircFormatToHtml(msg);
	// Add to an active session.
	if (addToActiveSession) {
		ChatSession *session = activeSession();
		if (session) {
			Message message(plainText);
			message.setChatUnit(session->getUnit());
			message.setIncoming(true);
			message.setProperty("service", true);
			message.setProperty("html", html);
			message.setTime(QDateTime::currentDateTime());
			session->appendMessage(message);
		}
	}
	// Add to the account console.
	QString str = QString("[%1] ").arg(QTime::currentTime().toString(Qt::SystemLocaleShortDate));
	if (!type.isEmpty()) {
		QString color = d->logMsgColors.value(type);
		if (!color.isEmpty())
			str += QString("<font color='%1'>[%2] %3 </font>").arg(color).arg(type).arg(html);
		else
			str += QString("[%1] %2").arg(type).arg(html);
	} else {
		str += html;
	}
	if (d->consoleForm)
		d->consoleForm.data()->appendMessage(str);
	if (!d->log.isEmpty())
		d->log += "<br>";
	d->log += str;
}

bool IrcAccount::isUserInputtedCommand(const QString &command, bool clearCommand)
{
	d->removeOldCommands();
	int i = 0;
	foreach (const LastCommand &itr, d->lastCommands) {
		if (command == itr.cmd) {
			if (clearCommand)
				d->lastCommands.removeAt(i);
			return true;
		}
		++i;
	}
	return false;
}

void IrcAccountPrivate::removeOldCommands()
{
	uint curTime = QDateTime::currentDateTime().toTime_t();
	int j = 0;
	for (int c = lastCommands.count(); j < c; ++j) {
		if (curTime - lastCommands.at(j).time < 30)
			break;
	}
	if (j != 0)
		lastCommands = lastCommands.mid(j);
}

IrcGroupChatManager *IrcAccount::groupChatManager()
{
	return reinterpret_cast<IrcGroupChatManager*>(Account::groupChatManager());
}

void IrcAccount::registerLogMsgColor(const QString &type, const QString &color)
{
	IrcAccountPrivate::logMsgColors.insert(type, color);
}

void IrcAccount::updateSettings()
{
	d->conn->loadSettings();
	emit settingsUpdated();
}

void IrcAccount::showConsole()
{
	if (d->consoleForm) {
		d->consoleForm.data()->raise();
	} else {
		d->consoleForm = new IrcConsoleFrom(this, d->log);
		d->consoleForm.data()->setAttribute(Qt::WA_DeleteOnClose);
		centerizeWidget(d->consoleForm.data());
		d->consoleForm.data()->show();
	}
}

void IrcAccount::showChannelList()
{
	if (d->channelListForm) {
		d->channelListForm.data()->raise();
	} else {
		d->channelListForm = new IrcChannelListForm(this);
		d->channelListForm.data()->setAttribute(Qt::WA_DeleteOnClose);
		centerizeWidget(d->channelListForm.data());
		d->channelListForm.data()->show();
	}
}

bool IrcAccount::event(QEvent *ev)
{
	return Account::event(ev);
}

void IrcAccount::onContactNickChanged(const QString &nick, const QString &oldNick)
{
	Q_ASSERT(qobject_cast<IrcContact*>(sender()));
	IrcContact *contact = static_cast<IrcContact*>(sender());
	d->contacts.remove(oldNick);
	Q_ASSERT(contact->id() == nick);
	d->contacts.insert(nick, contact);
}

} } // namespace qutim_sdk_0_3::irc

