/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include "jcontactresource.h"
#include <qutim/contact.h>
//Jreen
#include <jreen/presence.h>

namespace Jabber
{
class JContactResourcePrivate
{
public:
	JContactResourcePrivate(QObject *c) :
		contact(c),
	    presence(Jreen::Presence::Unavailable,Jreen::JID(c->property("id").toString())) {}
	QObject *contact;
	QString id;
	QString name;
	Jreen::Presence presence;
	QSet<QString> features;
	QHash<QString, QVariantHash> extInfo;
//	QCA::PGPKey pgpKey;
//	QCA::SecureMessageSignature::IdentityResult pgpVerifyStatus;
};
}

#endif // JCONTACTRESOURCE_P_H

