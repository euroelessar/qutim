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

#include "ircwhoisreplieshandler.h"
#include "irccontact.h"
#include "ircaccount.h"
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace irc {

IrcWhoisRepliesHandler::IrcWhoisRepliesHandler(QObject *parent) :
	QObject(parent)
{
	m_cmds
		<< 311  // RPL_WHOISUSER
		<< 314  // RPL_WHOWASUSER
		<< 312  // RPL_WHOISSERVER
		<< 313  // RPL_WHOISOPERATOR
		<< 317  // RPL_WHOISIDLE
		<< 318  // RPL_ENDOFWHOIS
		<< 319  // RPL_WHOISCHANNELS
		<< 330  // WHOISLOGGEDINAS
		<< 671; // WHOISSECURITECONNECTION
}

IrcWhoisRepliesHandler::~IrcWhoisRepliesHandler()
{
}

void IrcWhoisRepliesHandler::handleMessage(class IrcAccount *account, const QString &name, const QString &host,
										const IrcCommand &cmd, const QStringList &params)
{
	Q_UNUSED(host);
	if (cmd == 311 || cmd == 314) { // RPL_WHOISUSER or RPL_WHOWASUSER
		QString nick = params.value(1);
		QString user = params.value(2);
		QString host = params.value(3);
		QString realName = params.value(5);
		account->log(tr("%1 is %2@%3 (%4)")
					   .arg(nick)
					   .arg(user)
					   .arg(host)
					   .arg(realName),
					   account->isUserInputtedCommand("WHOIS"), "Whois");
		IrcContact *contact = account->getContact(nick, false);
		if (contact) {
			contact->setHost(host);
			contact->setHostUser(user);
			contact->setRealName(realName);
		}
	} else if (cmd == 312) { // RPL_WHOISSERVER
		account->log(tr("%1 is online via %2 (%3)")
					   .arg(params.value(1))
					   .arg(params.value(2))
					   .arg(params.value(3)),
					   account->isUserInputtedCommand("WHOIS"), "Whois");
	} else if (cmd == 313) { // RPL_WHOISOPERATOR
		QString nick = params.value(1);
		account->log(tr("%1 is an IRC operator").arg(nick),
					   account->isUserInputtedCommand("WHOIS"), "Whois");
		IrcContact *contact = account->getContact(nick, false);
		if (contact)
			contact->handleMode("ChanServ", "o", "");
	} else if (cmd == 317) { // RPL_WHOISIDLE
		QString nick = params.value(1);
		bool userRequest = account->isUserInputtedCommand("WHOIS");
		QDateTime idleSince = QDateTime::currentDateTime();
		idleSince.addSecs(-params.value(2).toInt());
		account->log(tr("%1 has been idle since")
					   .arg(nick)
					   .arg(idleSince.toString(Qt::DefaultLocaleShortDate)),
					   userRequest, "Whois");
		if (uint signOnUnix = params.value(3).toUInt()) {
			QDateTime signOn = QDateTime::fromTime_t(signOnUnix);
			account->log(tr("%1 has been online since")
						   .arg(name)
						   .arg(signOn.toString(Qt::DefaultLocaleShortDate)),
						   userRequest, "Whois");
		}
	} else if (cmd == 318) { // RPL_ENDOFWHOIS
		// Nothing to do...
	} else if (cmd == 319) { // RPL_WHOISCHANNELS
		QString str = tr("%1 is a user on channels: ").arg(params.value(1));
		QStringList channels = params;
		channels.removeFirst();
		channels.removeFirst();
		str += channels.join(", ");
		account->log(str, account->isUserInputtedCommand("WHOIS"), "Whois");
	} else if (cmd == 330) { // WHOISLOGGEDINAS
		account->log(tr("%1 is logged in as %2")
					   .arg(params.value(1))
					   .arg(params.value(2)),
					   account->isUserInputtedCommand("WHOIS"), "Whois");
	} else if (cmd == 671) { // WHOISSECURITECONNECTION
		account->log(tr("%1 is using a secure connection").arg(params.value(1)),
					   account->isUserInputtedCommand("WHOIS"), "Whois");
	}
}

} } // namespace qutim_sdk_0_3::irc

