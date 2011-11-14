/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCCOMMANDALIAS_H
#define IRCCOMMANDALIAS_H

#include "ircglobal.h"
#include <QSharedDataPointer>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcCommandAliasPrivate;

class IrcCommandAlias
{
public:
	enum Type {
		Disabled     = 0x0000,
		Channel      = 0x0001,
		PrivateChat  = 0x0002,
		Console      = 0x0004,
		Participant  = 0x0008,
		All          = Channel | PrivateChat | Console | Participant
	};
	Q_DECLARE_FLAGS(Types, Type);
	IrcCommandAlias(const QString &name, const QString &command, Types types = All);
	virtual ~IrcCommandAlias();
	QString name() const;
	virtual QString generate(IrcCommandAlias::Type aliasType, const QStringList &params,
							 const QHash<QChar, QString> &extParams, QString *error = 0) const;
	static void initStandartAliases();
private:
	Q_DISABLE_COPY(IrcCommandAlias);
	QScopedPointer<IrcCommandAliasPrivate> d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(IrcCommandAlias::Types);

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCOMMANDALIAS_H

