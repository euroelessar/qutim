/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef DOMAININFO_P_H
#define DOMAININFO_P_H

#include "domaininfo.h"
#include "../3rdparty/jdns/qjdns.h"

namespace qutim_sdk_0_3
{
	class DomainInfoHelper : public QObject
	{
		Q_OBJECT
	public:
		DomainInfoHelper();
		~DomainInfoHelper();
		void lookup(const QString &service, const QString &proto,
					const QString &domain, DomainInfo *handler);
	private slots:
		void onResultsReady(int id, const QJDns::Response &results);
		void onError(int id, QJDns::Error error);
	private:
		QMap<int, DomainInfo*> m_handlers;
	};
}

#endif // DOMAININFO_P_H
