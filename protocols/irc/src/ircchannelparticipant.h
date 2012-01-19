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

#ifndef IRCCHANNELPARTICIPANT_H
#define IRCCHANNELPARTICIPANT_H

#include <qutim/buddy.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcChannelParticipantPrivate;
class IrcChannel;
class IrcContact;
class IrcAccount;

class IrcChannelParticipant : public Buddy
{
	Q_OBJECT
public:
	enum IrcParticipantFlag
	{
		Voice,
		HalfOp,
		Op
	};
	Q_DECLARE_FLAGS(IrcParticipantFlags, IrcParticipantFlag);
	IrcChannelParticipant(IrcChannel *channel, const QString &nick, const QString &host);
	~IrcChannelParticipant();
	virtual bool sendMessage(const Message &message);
	virtual QString id() const;
	virtual QString name() const;
	virtual QString avatar() const;
	virtual Status status() const;
	IrcContact *contact();
	const IrcContact *contact() const;
	IrcChannel *channel();
	const IrcChannel *channel() const;
	IrcAccount *account();
	const IrcAccount *account() const;
	void setFlag(QChar flag);
	void setMode(QChar mode);
	void removeMode(QChar mode);
	QString hostMask() const; // ~sokol@h255-255-255-255.net.pnz.ru
	QString hostUser() const; // ~sokol
	QString domain() const; // h255-255-255-255.net.pnz.ru
	QString host() const; // net.pnz.ru
signals:
	void quit(const QString &message);
private:
	QScopedPointer<IrcChannelParticipantPrivate> d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(IrcChannelParticipant::IrcParticipantFlags)

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNELPARTICIPANT_H

