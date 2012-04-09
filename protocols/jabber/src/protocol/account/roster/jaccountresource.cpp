/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jaccountresource.h"
#include "../jaccount.h"

namespace Jabber
{

using namespace qutim_sdk_0_3;

class JAccountResourcePrivate
{
public:
	JContactResource *resource;
};

JAccountResource::JAccountResource(JAccount *account, const QString &id, const QString &resource)
    : JContact(id, account), m_resource(QLatin1Char('/') + resource)
{
	setContactTags(QStringList() << QLatin1String("My resources"));
	setContactInList(true);
	setContactSubscription(Jreen::RosterItem::Both);
	setContactName(account->name() + m_resource);
	connect(account, SIGNAL(nameChanged(QString,QString)),
	        SLOT(onNameChanged(QString)));
}

JAccountResource::~JAccountResource()
{
}

void JAccountResource::setName(const QString &name)
{
	Q_UNUSED(name);
}

void JAccountResource::setTags(const QStringList &tags)
{
	Q_UNUSED(tags);
}

void JAccountResource::setInList(bool inList)
{
	Q_UNUSED(inList);
}

void JAccountResource::onNameChanged(const QString &name)
{
	setContactName(name + m_resource);
}

} // namespace Jabber
