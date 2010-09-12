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
	IrcContact(IrcAccount *account, const QString &nick);
	~IrcContact();
	virtual QString id() const;
	virtual bool sendMessage(const Message &message);
	virtual QString avatar() const;
	void setAvatar(const QString &avatar);
	const IrcAccount *account() const;
	IrcAccount *account();
	QSet<QChar> modes();
signals:
	void quit(const QString &message);
private slots:
	void onSessionDestroyed();
private:
	friend class IrcContactPrivate;
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	void handleMode(const QString &who, const QString &mode, const QString &param);
	QScopedPointer<IrcContactPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONTACT_H
