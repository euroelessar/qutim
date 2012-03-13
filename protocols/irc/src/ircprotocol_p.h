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

#ifndef IRQPROTOCOL_P_H
#define IRQPROTOCOL_P_H

#include "ircaccount.h"
#include "ircprotocol.h"
#include "irccommandalias.h"
#include <qutim/chatsession.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcProtocolPrivate
{
public:
	inline IrcProtocolPrivate() :
		accounts_hash(new QHash<QString, QWeakPointer<IrcAccount> > ())
	{ }
	inline ~IrcProtocolPrivate() { delete accounts_hash; }
	static QString getColorByMircCode(const QString &code);
	union
	{
		QHash<QString, QWeakPointer<IrcAccount> > *accounts_hash;
		QHash<QString, IrcAccount *> *accounts;
	};
	QWeakPointer<ChatSession> activeSession;
	ActionGenerator *autojoinAction;
	static bool enableColoring;
	static QMultiHash<QString, IrcCommandAlias*> aliases;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCPROTOCOL_P_H

