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

#ifndef IRCAVATAR_H
#define IRCAVATAR_H

#include "ircctcphandler.h"
#include <QNetworkAccessManager>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcContact;
class IrcAccount;

class IrcAvatar : public QObject, public IrcCtcpHandler
{
	Q_OBJECT
public:
	IrcAvatar(QObject *parent = 0);
	void requestAvatar(IrcContact *contact);
	static IrcAvatar *instance() { if (!self) self = new IrcAvatar; return self; }
protected:
	virtual void handleCtcpRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
								   const QString &receiver, const QString &cmd, const QString &params);
	virtual void handleCtcpResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
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

