/****************************************************************************
 *  irqprotocol_p.h
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

#ifndef IRQPROTOCOL_P_H
#define IRQPROTOCOL_P_H

#include "ircaccount.h"
#include "ircprotocol.h"
#include <qutim/messagesession.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcProtocolPrivate
{
public:
	inline IrcProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<IrcAccount> > ())
	{ }
	inline ~IrcProtocolPrivate() { delete accounts_hash; }
	static QString getColorByMircCode(const QString &code);
	union
	{
		QHash<QString, QPointer<IrcAccount> > *accounts_hash;
		QHash<QString, IrcAccount *> *accounts;
	};
	QPointer<ChatSession> activeSession;
	static bool enableColoring;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCPROTOCOL_P_H
