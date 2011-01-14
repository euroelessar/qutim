/****************************************************************************
 *  irccontact_p.h
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

#ifndef IRCCONTACT_P_H
#define IRCCONTACT_P_H

#include <irccontact.h>
#include <QSet>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcContactPrivate
{
public:
	void ref();
	void deref();
	void updateNick(const QString &nick);
private:
	IrcContactPrivate() :
		m_ref(0)
	{}
	friend class IrcContact;
	IrcContact *q;
	QString nick;
	QSet<QChar> modes;
	QString avatar;
	QString awayMsg;
	mutable QAtomicInt m_ref;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONTACT_P_H
