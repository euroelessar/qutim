/****************************************************************************
 *  irccontact.h
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
	virtual QString id() const;
	virtual bool sendMessage(const Message &message);
	virtual QString avatar() const;
	virtual Status status() const;
	void setAvatar(const QString &avatar);
	const IrcAccount *account() const;
	IrcAccount *account();
	QSet<QChar> modes();
	QString hostMask() const; // ~sokol@h255-255-255-255.net.pnz.ru
	QString domain() const; // h255-255-255-255.net.pnz.ru
	QString host() const; // net.pnz.ru
signals:
	void quit(const QString &message);
private slots:
	void onSessionDestroyed();
private:
	friend class IrcContactPrivate;
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	friend class IrcAccount;
	void handleMode(const QString &who, const QString &mode, const QString &param);
	void setAway(const QString &awayMsg);
	void setHost(const QString &host);
	QScopedPointer<IrcContactPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONTACT_H
