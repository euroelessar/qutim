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
#include "abstractsearchrequest_p.h"
#include <QSet>

namespace qutim_sdk_0_3
{
	AbstractSearchRequest::AbstractSearchRequest()
	{
	}

	AbstractSearchRequest::~AbstractSearchRequest()
	{
	}

	QSet<QString> AbstractSearchRequest::services() const
	{
		return QSet<QString>();
	}

	void AbstractSearchRequest::setService(const QString &service)
	{
		Q_UNUSED(service);
	}

	AbstractSearchFactory::AbstractSearchFactory(AbstractSearchFactoryPrivate *d) :
		d_ptr(d)
	{
	}

	AbstractSearchFactory::AbstractSearchFactory()
	{
	}

	AbstractSearchFactory::~AbstractSearchFactory()
	{
	}

	QVariant AbstractSearchFactory::data(const QString &request, int role)
	{
		if (role == Qt::DisplayRole)
			return request;
		return QVariant();
	}

}

