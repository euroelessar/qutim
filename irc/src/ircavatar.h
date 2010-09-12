/****************************************************************************
 *  ircavatar.h
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

#ifndef IRCAVATAR_H
#define IRCAVATAR_H

#include "ircctpchandler.h"
#include <QNetworkAccessManager>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcContact;
class IrcAccount;

class IrcAvatar : public QObject, public IrcCtpcHandler
{
	Q_OBJECT
public:
	IrcAvatar(QObject *parent = 0);
	void requestAvatar(IrcContact *contact);
	static IrcAvatar *instance() { if (!self) self = new IrcAvatar; return self; }
protected:
	virtual void handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
								   const QString &receiver, const QString &cmd, const QString &params);
	virtual void handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
									const QString &receiver, const QString &cmd, const QString &params);
private slots:
	void avatarReceived(QNetworkReply *reply);
private:
	QString getAvatarDir() const;
	QNetworkAccessManager m_manager;
	static IrcAvatar *self;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCAVATAR_H
