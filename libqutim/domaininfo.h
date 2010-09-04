/****************************************************************************
 *  domaininfo.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
