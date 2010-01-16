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

	class LIBQUTIM_EXPORT DomainInfo
	{
	public:
		enum Type
		{
			None,
			A, Aaaa,
			Mx, Srv,
			Cname,
			Ptr,
			Txt
		};
		struct Record
		{
			QHostAddress address;    // for A, Aaaa
			QByteArray name;         // for Mx, Srv, Cname, Ptr, Ns
			int priority;            // for Mx, Srv
			int weight;              // for Srv
			int port;                // for Srv
			QList<QByteArray> texts; // for Txt
			QByteArray cpu;          // for Hinfo
			QByteArray os;           // for Hinfo
		};
	//	DomainInfo(const DomainInfo &d);
	//	DomainInfo &operator=(const DomainInfo &d);
	//	~DomainInfo();
	//	QList<Record> records() const;
	//	int id() const;
	//	Type type() const;
	//	bool isValid() const;
	//private:
	//	DomainInfo(int id);
	//	DomainInfoPrivate *p;
	};
}

#endif // DOMAININFO_H
