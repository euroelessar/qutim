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

#include "irccommandalias.h"
#include <QRegExp>
#include <QStringList>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcCommandAliasPrivate : public QSharedData
{
public:
	IrcCommandAliasPrivate(const QString &_name, const QString &_command, IrcCommandAlias::Types _types);
	QString name;
	QString command;
	IrcCommandAlias::Types types;
};

IrcCommandAliasPrivate::IrcCommandAliasPrivate(const QString &_name,
						const QString &_command, IrcCommandAlias::Types _types) :
	name(_name),
	command(_command),
	types(_types)
{
}

IrcCommandAlias::IrcCommandAlias(const QString &name, const QString &command, Types types) :
	d(new IrcCommandAliasPrivate(name, command, types))
{
}

IrcCommandAlias::~IrcCommandAlias()
{
}

QString IrcCommandAlias::name() const
{
	return d->name;
}

QString IrcCommandAlias::generate(IrcCommandAlias::Type aliasType, const QStringList &params,
								  const QHash<QChar, QString> &extParams, QString *error) const
{
	if (!(d->types & aliasType))
		return QString();
	static QRegExp paramRx("%([0-9]{1,2}|[a-zA-Z])(-|)");
	QString command = d->command; // the new command
	int pos = 0;
	while ((pos = paramRx.indexIn(command, pos)) != -1) {
		bool isIndex;
		int index = paramRx.cap(1).toInt(&isIndex);
		QString param; // next parameter in the new command
		if (isIndex) {
			// Replace template by parameter(s) from the command line
			Q_ASSERT(index >= 0);
			if (index >= params.size()) {
				if (error) *error = QObject::tr("Not enough parameters for command %1").arg(d->name);
				return QString();
			}
			if (paramRx.cap(2) != "-") {
				param = params.at(index);
			} else {
				for (int i = index, c = params.size(); i < c; ++i) {
					if (i != index)
						param += " ";
					param += params.at(i);
				}
			}
		} else {
			// Replace template by built-in parameter
			QChar extParamIndex = paramRx.cap(1).at(0);
			if (extParams.contains(extParamIndex)) {
				param = extParams.value(extParamIndex);
			} else {
				pos += paramRx.matchedLength();
				continue;
			}
		}
		command.replace(pos, paramRx.matchedLength(), param);
		pos += param.size();
	}
	return command;
}

} } // namespace qutim_sdk_0_3::irc
