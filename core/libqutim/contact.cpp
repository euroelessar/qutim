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

#include "contact_p.h"
#include "account.h"
#include "icon.h"
#include "metacontact.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{

Contact::Contact(Account *account) : Buddy(*new ContactPrivate(this), account)
{
}

Contact::Contact(ContactPrivate &d, Account *account) : Buddy(d, account)
{
}

Contact::~Contact()
{
}

QStringList Contact::tags() const
{
	return QStringList();
}

ChatUnit *Contact::upperUnit()
{
	return d_func()->metaContact.data();
}

bool Contact::event(QEvent *e)
{
	if (e->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(e);
		MetaContact *metaContact = metaEvent->newMetaContact();
		if (metaContact != this)
			d_func()->metaContact = metaContact;
	}
	return Buddy::event(e);
}

ContactComparator *ContactComparator::instance()
{
	static ServicePointer<ContactComparator> self;
	return self.data();
}

}
