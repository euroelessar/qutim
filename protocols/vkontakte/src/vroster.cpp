/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vroster.h"
#include <vk/roster.h>
#include "vcontact.h"
#include "vaccount.h"

VRoster::VRoster(VAccount *account) : QObject(account),
	m_account(account)
{
}

VContact *VRoster::contact(const QString &id)
{
	VContact *c = m_contactHash.value(id);
	if (!c) {
		vk::Buddy *buddy = static_cast<vk::Buddy*>(m_account->roster()->contact(id.toInt()));
		c = new VContact(buddy, m_account);
		connect(c, SIGNAL(destroyed(QObject*)), SLOT(onContactDestroyed(QObject*)));
		m_contactHash.insert(c->id(), c);
		emit m_account->contactCreated(c);
	}
	return c;
}

VContact *VRoster::contact(const QString &id) const
{
	return m_contactHash.value(id);
}

void VRoster::onContactDestroyed(QObject *obj)
{
	m_contactHash.remove(m_contactHash.key(static_cast<VContact*>(obj)));
}


