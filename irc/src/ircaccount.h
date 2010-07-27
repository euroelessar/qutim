/****************************************************************************
 *  ircaccount.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

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

class IrcAccount: public Account
{
	Q_OBJECT
public:
	IrcAccount(const QString &network);
	virtual ~IrcAccount();
	virtual void setStatus(Status status);
	virtual QString name() const;
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	IrcChannel *getChannel(const QString &name, bool create = false);
	IrcContact *getContact(const QString &nick, bool create = false);
	void send(const QString &command, IrcCommandAlias::Type aliasType = IrcCommandAlias::Disabled,
			  const QHash<QChar, QString> &extParams = QHash<QChar, QString>()) const;
	void sendCtpcRequest(const QString &contact, const QString &cmd, const QString &params);
	void sendCtpcReply(const QString &contact, const QString &cmd, const QString &params);
	void setName(const QString &name) const;
	IrcProtocol *protocol();
	const IrcProtocol *protocol() const;
	ChatSession *activeSession() const;
	void log(const QString &msg, bool addToActiveSession = false, const QString &type = QString());
	static QString ircFormatToHtml(const QString &msg, QString *plainText = 0);
	static QString ircFormatToPlainText(const QString &msg);
	static void registerLogMsgColor(const QString &type, const QString &color);
public slots:
	void updateSettings();
	void showConsole();
	void showChannelList();
protected:
	virtual bool event(QEvent *ev);
private slots:
	void onContactRemoved();
	void onContactNickChanged(const QString &nick);
private:
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	QScopedPointer<IrcAccountPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNT_H
