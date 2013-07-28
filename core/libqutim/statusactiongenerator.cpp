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
#include "statusactiongenerator_p.h"
#include "account.h"

namespace qutim_sdk_0_3
{
Q_GLOBAL_STATIC(StatusActionHandler, statusActionHandler);

StatusActionHandler::StatusActionHandler(QObject *parent) :
	QObject(parent), m_memberName(SLOT(changeStatus(QAction*,QObject*)))
{
}

void StatusActionHandler::changeStatus(QAction *action, QObject *controller)
{
	if (Account *account = qobject_cast<Account *>(controller)) {
		Status current = account->status();
		Status origin = action->property("status").value<Status>();
		current.setType(origin.type());
		current.setSubtype(origin.subtype());
		current.setChangeReason(Status::ByUser);
		account->setStatus(current);
	}
}

StatusActionGenerator::StatusActionGenerator(const Status &status) :
	ActionGenerator(*new StatusActionGeneratorPrivate)
{
	Q_D(StatusActionGenerator);
	d->q_ptr = this;
	d->data = new ActionData;
	d->status = status;
	if (d->status.icon().isNull()) {
		QIcon icon = Status::createIcon(status.type());
		if (!icon.isNull())
			d->status.setIcon(icon);
	}
	d->receiver = statusActionHandler();
	d->member = statusActionHandler()->memberName();
	d->ensureConnectionType();
	d->icon = d->status.icon();
	d->text = d->status.name();
	d->priority = -status.type();
}

QObject *StatusActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setProperty("status", QVariant::fromValue(d_func()->status));
	return action;
}
}

