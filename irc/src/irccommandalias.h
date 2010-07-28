/****************************************************************************
 *  irccommandalias.h
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

#ifndef IRCCOMMANDALIAS_H
#define IRCCOMMANDALIAS_H

#include <ircglobal.h>
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
		All          = Channel | PrivateChat | Console
				   };
	Q_DECLARE_FLAGS(Types, Type);
	IrcCommandAlias(const QString &name, const QString &command, Types types = All);
	virtual ~IrcCommandAlias();
	QString name() const;
	virtual QString generate(IrcCommandAlias::Type aliasType, const QStringList &params,
							 const QHash<QChar, QString> &extParams, QString *error = 0) const;
private:
	Q_DISABLE_COPY(IrcCommandAlias);
	QScopedPointer<IrcCommandAliasPrivate> d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(IrcCommandAlias::Types);

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCOMMANDALIAS_H
