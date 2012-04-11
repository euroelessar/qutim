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

#ifndef IRCCONTACT_H
#define IRCCONTACT_H

#include <QSharedData>
#include <qutim/buddy.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;
class IrcContactPrivate;

class IrcContact : public Buddy
{
	Q_OBJECT
public:
	IrcContact(IrcAccount *account, const QString &nick, const QString &host);
	~IrcContact();

	void ref();
	void deref();

	virtual QString id() const;
	virtual bool sendMessage(const Message &message);
	virtual QString avatar() const;
	virtual Status status() const;
	void setAvatar(const QString &avatar);
	const IrcAccount *account() const;
	IrcAccount *account();
	QSet<QChar> modes();
	QString hostMask() const; // ~sokol@h255-255-255-255.net.pnz.ru
	QString hostUser() const; // ~sokol
	QString domain() const; // h255-255-255-255.net.pnz.ru
	QString host() const; // net.pnz.ru
	QString realName() const;
signals:
	void quit(const QString &message);
private slots:
	void destroyLater();
private:
	friend class IrcContactPrivate;
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	friend class IrcAccount;
	friend class IrcWhoisRepliesHandler;
	void handleMode(const QString &who, const QString &mode, const QString &param);
	void setAway(const QString &awayMsg);
	void setHostMask(const QString &hostMask);
	void setHost(const QString &host, int pos = 0);
	void setHostUser(const QString &user);
	void setRealName(const QString &name);
	QScopedPointer<IrcContactPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONTACT_H

