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

#ifndef IRCACCOUNT_H
#define IRCACCOUNT_H

#include <qutim/account.h>
#include <QHostAddress>
#include "irccontact.h"
#include "ircprotocol.h"

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcAccountPrivate;
class IrcChannel;
class IrcGroupChatManager;

typedef QHash<QChar, QString> ExtendedParams;

class IrcAccount: public Account
{
	Q_OBJECT
	Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
public:
	IrcAccount(const QString &network);
	virtual ~IrcAccount();
	virtual void setStatus(Status status);
	virtual QString name() const;
	QString avatar();
	void setAvatar(const QString &avatar);
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	IrcChannel *getChannel(const QString &name, bool create = false);
	IrcContact *getContact(const QString &nick, const QString &host, bool create = false);
	IrcContact *getContact(const QString &nick, bool create = false);
	void removeContact(const QString &id);
	void removeChannel(const QString &id);
	void send(const QString &command, bool highPriority = true,
			  IrcCommandAlias::Type aliasType = IrcCommandAlias::Disabled,
			  const ExtendedParams &extParams = ExtendedParams()) const;
	void send(IrcCommandAlias *alias, bool highPriority = true,
			  IrcCommandAlias::Type aliasType = IrcCommandAlias::Disabled,
			  QStringList params = QStringList(),
			  const ExtendedParams &extParams = ExtendedParams()) const;
	void sendCtpcRequest(const QString &contact, const QString &cmd,
						 const QString &param, bool highPriority = true);
	void sendCtpcReply(const QString &contact, const QString &cmd,
					   const QString &params, bool highPriority = true);
	void setName(const QString &name) const;
	IrcProtocol *protocol();
	const IrcProtocol *protocol() const;
	ChatSession *activeSession() const;
	void log(const QString &msg, bool addToActiveSession = false, const QString &type = QString()) const;
	bool isUserInputtedCommand(const QString &command, bool clearCommand = false);
	IrcGroupChatManager *groupChatManager();
	static void registerLogMsgColor(const QString &type, const QString &color);
signals:
	void avatarChanged(const QString &avatar);
	void settingsUpdated();
public slots:
	void updateSettings();
	void showConsole();
	void showChannelList();
protected:
	virtual bool event(QEvent *ev);
private slots:
	void onContactNickChanged(const QString &nick, const QString &oldNick);
private:
	friend class IrcContact;
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	friend class IrcChannel;
	friend class IrcProxyManager;
	QScopedPointer<IrcAccountPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNT_H

