/****************************************************************************
 *  ircchannelparticipant.h
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
	IrcChannelParticipant(IrcChannel *channel, const QString &nick);
	~IrcChannelParticipant();
	virtual bool sendMessage(const Message &message);
	virtual QString id() const;
	virtual QString name() const;
	virtual QString avatar() const;
	IrcContact *contact();
	const IrcContact *contact() const;
	IrcChannel *channel();
	const IrcChannel *channel() const;
	IrcAccount *account();
	const IrcAccount *account() const;
	void setFlag(QChar flag);
	void setMode(QChar mode);
	void removeMode(QChar mode);
signals:
	void quit(const QString &message);
private:
	QScopedPointer<IrcChannelParticipantPrivate> d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(IrcChannelParticipant::IrcParticipantFlags)

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNELPARTICIPANT_H
