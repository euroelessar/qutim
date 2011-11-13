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

#ifndef DOMAININFO_H
#define DOMAININFO_H

#include "libqutim_global.h"
#include <QHostAddress>

namespace qutim_sdk_0_3
{
	class DomainInfoPrivate;

	class LIBQUTIM_EXPORT DomainInfo : public QObject
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(DomainInfo)
	public:
		struct Record
		{
			QString name;
			int port;
			int weight;
		};

		DomainInfo(QObject *parent = 0);
		~DomainInfo();
		
		void lookupSrvRecord(const QString &service, const QString &proto, const QString &domain);
		QList<Record> resultRecords();
		
	signals:
		void resultReady();
		
	private:
		QScopedPointer<DomainInfoPrivate> d_ptr;
	};
}

#endif // DOMAININFO_H

