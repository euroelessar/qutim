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

#include "metacontactmanager.h"
#include "metacontactprotocol_p.h"
#include "account_p.h"
#include "metacontact.h"
#include "servicemanager.h"
#include <QUuid>

namespace qutim_sdk_0_3
{
class MetaContactManagerPrivate : public AccountPrivate
{
public:
	MetaContactManagerPrivate(Account *a) : AccountPrivate(a) {}
};

MetaContactManager::MetaContactManager()
	: Account(*new MetaContactManagerPrivate(this), new MetaProtocol(this))
{
	Q_D(MetaContactManager);
	d->id = QLatin1String("meta");
}

MetaContactManager::~MetaContactManager()
{
}

MetaContact *MetaContactManager::createContact()
{
	QString id = QUuid::createUuid().toString();
	MetaContact *contact = qobject_cast<MetaContact*>(getUnit(id, true));
	return contact;
}

MetaContactManager *MetaContactManager::instance()
{
	static ServicePointer<MetaContactManager> self;
	return self.data();
}

bool MetaContactManager::eventFilter(QObject *obj, QEvent *ev)
{
	return Account::eventFilter(obj, ev);
}
}

