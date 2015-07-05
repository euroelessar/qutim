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

#ifndef IRCCONTACT_P_H
#define IRCCONTACT_P_H

#include "irccontact.h"
#include <QSet>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcContactPrivate
{
public:
	void updateNick(const QString &nick);
private:
	IrcContactPrivate() :
		m_ref(0)
	{}
	friend class IrcContact;
	IrcContact *q;
	QString nick;
	QString hostMask;
	QString hostUser;
	QString domain;
	QString host;
	QSet<QChar> modes;
	QString avatar;
	QString awayMsg;
	QString realName;
	mutable int m_ref;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONTACT_P_H

