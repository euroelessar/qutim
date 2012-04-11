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

#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include <qutim/conference.h>
#include <qutim/notification.h>
#include "ircglobal.h"

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcChannelPrivate;
class IrcChannelParticipant;
class IrcAccount;

class IrcChannel : public Conference
{
	Q_OBJECT
public:
	IrcChannel(IrcAccount *account, const QString &name);
	~IrcChannel();
	virtual Buddy *me() const;
	using Conference::join;
	void join(const QString &pass);
	void leave(bool force = false);
	virtual QString id() const;
	virtual QString title() const;
	virtual bool sendMessage(const Message &message);
	virtual QString topic() const;
	virtual ChatUnitList lowerUnits();
	void setAutoJoin(bool autojoin = true);
	bool autoJoin();
	const IrcAccount *account() const;
	IrcAccount *account();
	IrcChannelParticipant *participant(const QString &nick);
	QList<IrcChannelParticipant*> participants();
signals:
	void autoJoinChanged(bool enabled);
protected:
	virtual void doJoin();
	virtual void doLeave();
private slots:
	void onMyNickChanged(const QString &nick);
	void onParticipantNickChanged(const QString &nick, const QString &oldNick);
	void onContactQuit(const QString &message);
private:
	void setBookmarkName(const QString &name);
	void handleUserList(const QStringList &users);
	void handleJoin(const QString &nick, const QString &host);
	void handlePart(const QString &nick, const QString &message);
	void handleKick(const QString &nick, const QString &by, const QString &message);
	void handleTopic(const QString &topic);
	void handleTopicInfo(const QString &user, const QString &timeStr);
	void handleMode(const QString &who, const QString &mode, const QString &param);
	void setMode(const QString &who, QChar mode, const QString &param);
	void removeMode(const QString &who, QChar mode, const QString &param);
	void addSystemMessage(const QString &message, const QString &sender = QString(),
						  Notification::Type type = Notification::System);
	void clear(ChatSession *session);

	friend class IrcConnection;
	friend class IrcGroupChatManager;

	QScopedPointer<IrcChannelPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNEL_H

