/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jjidvalidator.h"
#include <jreen/jid.h>

namespace Jabber
{
class JJidValidatorPrivate
{
public:
	QString server;
};

JJidValidator::JJidValidator(const QString &server, QObject *parent)
	: QValidator(parent), d_ptr(new JJidValidatorPrivate)
{
	d_func()->server = server;
}

JJidValidator::~JJidValidator()
{
}

QString JJidValidator::server() const
{
	return d_func()->server;
}

QValidator::State JJidValidator::validate(QString &str, int &) const
{
	Q_D(const JJidValidator);
	Jreen::JID jid;
	if (d->server.isEmpty()) {
		jid.setJID(str);
		if (str.indexOf('@') == str.size()-1)
			return Intermediate;
		if (!jid.isValid())
			return Invalid;
	} else {
		if (str.indexOf('@') != -1) {
			if (jid.setJID(str))
				jid.setDomain(d->server);
		} else {
			if (jid.setNode(str))
				jid.setDomain(d->server);
		}
		if (!jid.isValid())
			return Invalid;
	}
	fixup(str);
	return Acceptable;
}

void JJidValidator::fixup(QString &str) const
{
	Q_D(const JJidValidator);
	Jreen::JID jid;
	if (d->server.isEmpty()) {
		if (jid.setJID(str))
			str = jid.bare();
	} else {
		if (str.indexOf('@') != -1) {
			if (jid.setJID(str))
				jid.setDomain(d->server);
		} else {
			if (jid.setNode(str))
				jid.setDomain(d->server);
		}
		str = jid.isValid() ? jid.node() : str;
	}
}
}

